// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Widgets/Inventory/GridSlot/Inv_GridSlot.h"
#include "Components/CanvasPanel.h"
#include "InventoryManagment/Components/Inv_InventoryComponent.h"
#include "InventoryManagment/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Utils/Inv_WidgetUtils.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Widgets/Inventory/SlottedItems/Inv_SlottedItem.h"

void UInv_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	ConstructGridSlots();
	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	
	/*Listen to an event fired from UInv_InventoryComponent::Server_AddNewItem_Implementation*/
	InventoryComponent->OnItemAdded.AddDynamic(this, &UInv_InventoryGrid::AddItem);
	
	/*Listen to an event fired from UInv_InventoryComponent::TryAddItem*/
	InventoryComponent->OnStackChange.AddDynamic(this, &UInv_InventoryGrid::AddStacks);
	
}

void UInv_InventoryGrid::AddStacks(const FInv_SlotAvailabilityResult& Result)
{
	if (!MatchesCategory(Result.Item.Get())) return; /*Find the correct inventory*/
	
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		if (Availability.bItemAtIndex)
			/*if the item is in the provided index, just update the stack ui and inventory logic*/
		{
			const auto& GridSlot = GridSlotsArray[Availability.Index];
			const auto& SlottedItem = SlottedItemsMap.FindChecked(Availability.Index);
			SlottedItem->UpdateStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
			GridSlot->SetStackCount(GridSlot->GetStackCount() + Availability.AmountToFill);
		}
		else 
			/*if the item is not in the provided index, create the slotted item and update the grid slots*/
		{
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
			UpdateGridSlots(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
		}
	}
}

void UInv_InventoryGrid::OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	check(GridSlotsArray.IsValidIndex(GridIndex));
	UInv_InventoryItem* ClickedInventoryItem = GridSlotsArray[GridIndex]->GetInventoryItem().Get();
	
	if (!IsValid(HoverItem) && IsLeftClicked(MouseEvent))
	{
		PickUp(ClickedInventoryItem, GridIndex);
	}
}

bool UInv_InventoryGrid::IsRightClicked(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

bool UInv_InventoryGrid::IsLeftClicked(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

void UInv_InventoryGrid::PickUp(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	// Assign the hover item
	AssignHoverItem(ClickedInventoryItem);
	
	// Remove clicked item, from the grid
}

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem)
{
	if (!IsValid(InventoryItem)) return;
	
	if (!IsValid(HoverItem))
	{
		HoverItem = CreateWidget<UInv_HoverItem>(GetOwningPlayer(),HoverItemClass);
	}
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(InventoryItem, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment) return;
	
	const FVector2D DrawSize = GetDrawSize(GridFragment);
	FSlateBrush IconBrush;
	IconBrush.SetResourceObject(ImageFragment->GetIcon());
	IconBrush.DrawAs = ESlateBrushDrawType::Image;
	IconBrush.ImageSize = DrawSize * UWidgetLayoutLibrary::GetViewportScale(this);
	
	HoverItem->SetImageBruh(IconBrush);
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());
	
	if (GetOwningPlayer()) GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default,HoverItem);
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem_Grid_IC(const UInv_ItemComponent* ItemComponent)
{
	return HasRoomForItem_Grid_IM(ItemComponent->GetItemComponent_Manifest());
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem_Grid_II(const UInv_InventoryItem* InventoryItem)
{
	return HasRoomForItem_Grid_IM(InventoryItem->GetItemManifest());
}

FInv_SlotAvailabilityResult UInv_InventoryGrid::HasRoomForItem_Grid_IM(const FInv_ItemManifest& ItemManifest)
{
	FInv_SlotAvailabilityResult Result;
	
	// Determine if the item is stackable or not 
	const FInv_StackableFragment* StackableFragment = ItemManifest.GetFragmentOfType<FInv_StackableFragment>();
	Result.bStackable = StackableFragment!= nullptr;
	
	// Determine how many stacks to add 
	const int32 MaxStackSize = StackableFragment? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = StackableFragment? StackableFragment->GetStackCount() : 1;
	
	TSet<int32> CheckedIndices;
	
	// For each grid slot 
	for (const auto& GridSlot : GridSlotsArray)
	{
		// if we don't have anymore to fill, break out of the loop early 
		if (AmountToFill == 0) break;
		
		// Is this index claimed yet? 
		if (IsIndexClaimed(CheckedIndices,GridSlot->GetTileIndex())) continue;
		
		// Is the item in grid bounds?
		if (!IsInGridBounds(GridSlot->GetTileIndex(), GetItemDimensions(ItemManifest))) continue;
		
		// can the item fit here? (is it out of grid bounds?)
		TSet<int32> TentativelyClaimed;
		if (!HasRoomAtIndex(GridSlot, GetItemDimensions(ItemManifest),CheckedIndices, TentativelyClaimed, ItemManifest.GetItemType(), MaxStackSize))
		{
			continue;
		}
		
		// how much to fill?
		const int32 AmountToFillInSlot = DetermineFillAmountForSlot(Result.bStackable,MaxStackSize,AmountToFill,GridSlot);
		if (AmountToFillInSlot == 0) continue;
		
		CheckedIndices.Append(TentativelyClaimed);
		
		// update the amount left to fill
		Result.TotalRoomToFill += AmountToFillInSlot;
		Result.SlotAvailabilities.Emplace(
			FInv_SlotAvailability
			{
				HasValidItem(GridSlot) ? GridSlot->GetUpperLeftIndex() : GridSlot->GetTileIndex(),
				Result.bStackable ? AmountToFillInSlot : 0,
				HasValidItem(GridSlot)
			}
		);
		
		AmountToFill -= AmountToFillInSlot;
		
		// how much is the remainder
		Result.Remainder = AmountToFill;
		if (AmountToFill == 0) return Result;
	}
	
	return Result;
}

bool UInv_InventoryGrid::HasRoomAtIndex(
		const UInv_GridSlot* GridSlot,
		const FIntPoint& Dimensions,
		const TSet<int32>& CheckedIndices,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		const int32 MaxStackSize)
{
	// is there room at that index? (is there another item in the way?)
	bool bHasRoomAtIndex = true;
	
	UInv_InventoryStatics::ForEach2D(GridSlotsArray,GridSlot->GetTileIndex(), Dimensions, Columns, [&](const UInv_GridSlot* SubGridSlot)
	{
		if (CheckSlotConstrains(GridSlot, SubGridSlot, CheckedIndices,OutTentativelyClaimed,ItemType, MaxStackSize))
		{
			OutTentativelyClaimed.Add(SubGridSlot->GetTileIndex());
		}
		else
		{
			bHasRoomAtIndex = false;
		}
	});
	
	return bHasRoomAtIndex;
}

bool UInv_InventoryGrid::CheckSlotConstrains(
		const UInv_GridSlot* GridSlot, 
		const UInv_GridSlot* SubGridSlot, 
		const TSet<int32>& CheckedIndices,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		const int32 MaxStackSize) const
{
	// index claimed?
	if (IsIndexClaimed(CheckedIndices, SubGridSlot->GetTileIndex())) return false;
	
	// has a valid item?
	if (!HasValidItem(SubGridSlot))
	{
		OutTentativelyClaimed.Add(SubGridSlot->GetTileIndex());
		return true;
	}
	
	// Is This GridSlot an Upper Left Slot?
	if (!IsUpperLeftSlot(GridSlot, SubGridSlot)) return false;
	
	//is this a stackable item?
	UInv_InventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable()) return false;
	
	// is this item the same type as the item we're trying to add?
	if (!DoesItemTypeMatch(SubItem, ItemType)) return false;
	
	// if stackable, is this slot at the max stack size already?
	if (GridSlot->GetStackCount()>= MaxStackSize) return false;
	
	return true;
}

FIntPoint UInv_InventoryGrid::GetItemDimensions(const FInv_ItemManifest& ItemManifest) const
{
	const FInv_GridFragment* GridFragment = ItemManifest.GetFragmentOfType<FInv_GridFragment>();
	return GridFragment ? GridFragment->GetGridSize() : FIntPoint(1,1);
}

bool UInv_InventoryGrid::HasValidItem(const UInv_GridSlot* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}

bool UInv_InventoryGrid::IsUpperLeftSlot(const UInv_GridSlot* GridSlot, const UInv_GridSlot* SubGridSlot) const
{
	return SubGridSlot->GetUpperLeftIndex() == GridSlot->GetTileIndex();
}

bool UInv_InventoryGrid::DoesItemTypeMatch(const UInv_InventoryItem* SubItem, const FGameplayTag& ItemType) const
{
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

bool UInv_InventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const
{
	if (StartIndex < 0 || StartIndex >= GridSlotsArray.Num()) return false;
	const int32 EndColumn = (StartIndex % Columns) + ItemDimensions.X;
	const int32 EndRow = (StartIndex / Columns) + ItemDimensions.Y;
	return EndColumn<= Columns && EndRow <= Rows;
}

int32 UInv_InventoryGrid::DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize,
	const int32 AmountToFill, const UInv_GridSlot* GridSlot) const 
{
	// Calculate the room in the slot
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot); /*call the function to make sure we return the stack amount from the upper left slot*/
	
	// if stackable, we need the minimum between the amount to fill and room in the slot
	return bStackable? FMath::Min(AmountToFill, RoomInSlot) : 1;
}

int32 UInv_InventoryGrid::GetStackAmount(const UInv_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackCount = GridSlot->GetStackCount();
	
	// if we're at a slot that does not hold the stack count, we must get the actual stack count from the authority slot
	if (const int32 UpperLeftIndex = GridSlot->GetUpperLeftIndex(); UpperLeftIndex != INDEX_NONE)
	{
		const UInv_GridSlot* UpperLeftGridSlot = GridSlotsArray[UpperLeftIndex];
		CurrentSlotStackCount = UpperLeftGridSlot->GetStackCount();
	}
	return CurrentSlotStackCount;
}

void UInv_InventoryGrid::AddItem(UInv_InventoryItem* InventoryItem)
{
	if (!MatchesCategory(InventoryItem)) return;
	
	const FInv_SlotAvailabilityResult Result = HasRoomForItem_Grid_II(InventoryItem);
	AddItemToIndices(Result, InventoryItem);
}

bool UInv_InventoryGrid::MatchesCategory(const UInv_InventoryItem* InventoryItem) const
{
	return this->ItemCategory == InventoryItem->GetItemManifest().GetItemCategory();
}

void UInv_InventoryGrid::AddItemToIndices(const FInv_SlotAvailabilityResult& Result, UInv_InventoryItem* NewItem)
{
	for (const auto& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
		UpdateGridSlots(NewItem, Availability.Index, Result.bStackable,Availability.AmountToFill);
	}
}

void UInv_InventoryGrid::AddItemAtIndex(UInv_InventoryItem* Item, const int32 Index, const bool bStackable,
                                        const int32 StackAmount)
{
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(Item, FragmentTags::GridFragment);
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(Item, FragmentTags::IconFragment);
	if (!GridFragment || !ImageFragment) return;
	
	UInv_SlottedItem* SlottedItem = CreateSlottedItem(Item, bStackable, StackAmount, Index, GridFragment, ImageFragment);
	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);
	
	SlottedItemsMap.Add(Index, SlottedItem);
}

UInv_SlottedItem* UInv_InventoryGrid::CreateSlottedItem(UInv_InventoryItem* Item,const bool BStackable,
		const int32 StackAmount, const int32 Index, const FInv_GridFragment* GridFragment,
		const FInv_ImageFragment* ImageFragment)
{
	UInv_SlottedItem* SlottedItem = CreateWidget<UInv_SlottedItem>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	SlottedItem->SetGridIndex(Index);
	
	SlottedItem->SetIsStackable(BStackable);
	const int32 StackUpdateAmount = BStackable ? StackAmount : 0;
	SlottedItem->UpdateStackCount(StackUpdateAmount);
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &ThisClass::OnSlottedItemClicked);
	
	return SlottedItem;
}

void UInv_InventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FInv_GridFragment* GridFragment,
	UInv_SlottedItem* SlottedItem) const
{
	SlotsCanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));
	const FVector2D DrawPos = UInv_WidgetUtils::GetPositionFromIndex(Index, Columns) * TileSize;
	const FVector2D DrawPosWithPadding = DrawPos + FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(DrawPosWithPadding);
}

void UInv_InventoryGrid::UpdateGridSlots(UInv_InventoryItem* NewItem, const int32 Index, bool bStackableItem, const int32 StackAmount)
{
	check(GridSlotsArray.IsValidIndex(Index));
	
	if (bStackableItem)
	{
		GridSlotsArray[Index]->SetStackCount(StackAmount);
	}
	
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(NewItem, FragmentTags::GridFragment);
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1,1);
	
	UInv_InventoryStatics::ForEach2D(GridSlotsArray, Index,Dimensions, Columns,[&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(NewItem);
		GridSlot->SetUpperLeftIndex(Index);
		GridSlot->SetGridSlotAvailable(false);
		GridSlot->SetOccupiedTexture();
	} );
}

bool UInv_InventoryGrid::IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const
{
	return CheckedIndices.Contains(Index);
}

void UInv_InventoryGrid::SetSlottedItemImage(const UInv_SlottedItem* SlottedItem, const FInv_GridFragment* GridFragment,
                                             const FInv_ImageFragment* ImageFragment) const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}

FVector2D UInv_InventoryGrid::GetDrawSize(const FInv_GridFragment* GridFragment) const
{
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding()*2;
	return GridFragment->GetGridSize() * IconTileWidth;
}

void UInv_InventoryGrid::ConstructGridSlots()
{
	GridSlotsArray.Reserve(Rows*Columns);
	
	for (int32 j = 0; j<Rows; ++j)
	{
		for (int32 i = 0; i<Columns; ++i)
		{
			UInv_GridSlot* GridSlot = CreateWidget<UInv_GridSlot>(this, GridSlotClass);
			SlotsCanvasPanel->AddChildToCanvas(GridSlot);
			
			const FIntPoint TilePosition(i,j);
			GridSlot->SetTileIndex(UInv_WidgetUtils::GetIndexFromPosition(TilePosition,Columns));
			
			UCanvasPanelSlot* GridCps = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			GridCps->SetSize(FVector2D(TileSize));
			GridCps->SetPosition(TilePosition*TileSize);
			
			GridSlotsArray.Add(GridSlot);
			
		}
	}
}
