// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"
#include "InventoryPlugin.h"
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
#include "Widgets/ItemPopUp/Inv_ItemPopUp.h"

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

void UInv_InventoryGrid::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	const FVector2d CanvasPosition = UInv_WidgetUtils::GetWidgetPosition(SlotsCanvasPanel);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	
	if (CursorExistedCanvas(CanvasPosition, UInv_WidgetUtils::GetWidgetSize(SlotsCanvasPanel), MousePosition))
	{
		return;
	}
	UpdateTileParameters(CanvasPosition,MousePosition);
}

void UInv_InventoryGrid::UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition)
{
	if (!bMouseWithInCanvas) return; // early exit return if the mouse is outside canvas
	
	// Calculate the tile quadrant, index and coordinates
	const FIntPoint HoveredTileCoordinates = CalculateHoveredCoordinates(CanvasPosition, MousePosition);
	
	LastFrameTileParameters = CurrentFrameTileParameters;
	CurrentFrameTileParameters.TileCoordinates = HoveredTileCoordinates;
	CurrentFrameTileParameters.TileIndex = UInv_WidgetUtils::GetIndexFromPosition(HoveredTileCoordinates, Columns);
	CurrentFrameTileParameters.TileQuadrant = CalculateTileQuadrant(CanvasPosition,MousePosition);
	
	// handle highlight and unhighlight of the grid slot
	OnTileParametersUpdated(CurrentFrameTileParameters);
}

void UInv_InventoryGrid::OnTileParametersUpdated(const FInv_TileParameters& Parameters)
{
	if (!IsValid(HoverItem)) return;
	
	// Get Hover Item's Dimensions
	const FIntPoint HoverItemDimensions = HoverItem->GetGridDimensions();
	
	// Calculate the starting coordinates for highlighting
	const FIntPoint StartingCoordinate = CalculateStartingCoordinate(Parameters.TileCoordinates, HoverItemDimensions, Parameters.TileQuadrant);
	ItemDropIndex = UInv_WidgetUtils::GetIndexFromPosition(StartingCoordinate, Columns);
	
	// Checking hover item position
	CurrentQueryResult = CheckHoverPosition(StartingCoordinate, HoverItemDimensions);
	
	if (CurrentQueryResult.bHasSpace) // No Item Case, the space is free.
	{
		HighlightSlots(ItemDropIndex, HoverItemDimensions);
		return;
	}
	UnhighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	
	if (CurrentQueryResult.ValidInventoryItem.IsValid() && GridSlotsArray.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		//Swap or combine.
		const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(CurrentQueryResult.ValidInventoryItem.Get(),FragmentTags::GridFragment);
		if (!GridFragment) return;
		
		ChangeHoverType(CurrentQueryResult.UpperLeftIndex,GridFragment->GetGridSize(),EInv_GridSlotState::GrayedOut);
	}
}

FInv_SpaceQueryResult UInv_InventoryGrid::CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions)
{
	FInv_SpaceQueryResult Result;
	
	// in the grid bounds?
	if (!IsInGridBounds(UInv_WidgetUtils::GetIndexFromPosition(Position,Columns),Dimensions)) return Result;
	
	Result.bHasSpace = true;
	
	// if more than one of the indices with the same item, we need to see if they have the same left upper index
	TSet<int32> OccupiedUpperLeftIndices;
	UInv_InventoryStatics::ForEach2D(GridSlotsArray, UInv_WidgetUtils::GetIndexFromPosition(Position,Columns),
		Dimensions, Columns, [&](const UInv_GridSlot* GridSlot)
	{
		if (GridSlot->GetInventoryItem().IsValid())
		{
			OccupiedUpperLeftIndices.Add(GridSlot->GetUpperLeftIndex());
			Result.bHasSpace = false;
		}
	});
	
	// how many items in the way? (only one then we can swap)
	if (OccupiedUpperLeftIndices.Num() == 1) // single item at position, it's valid for swapping or combining
	{
		const int32 Index = *OccupiedUpperLeftIndices.CreateConstIterator();
		Result.ValidInventoryItem = GridSlotsArray[Index]->GetInventoryItem();
		Result.UpperLeftIndex = GridSlotsArray[Index]->GetUpperLeftIndex();
		
	}
	return Result;
}

bool UInv_InventoryGrid::CursorExistedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize,
	const FVector2D& MouseLocation)
{
	bLastMouseWithInCanvas = bMouseWithInCanvas;
	bMouseWithInCanvas = UInv_WidgetUtils::IsWithinBounds(BoundaryPos, BoundarySize, MouseLocation);
	if (!bMouseWithInCanvas && bLastMouseWithInCanvas)
	{
		UnhighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
		return true;
	}
	return false;
}

void UInv_InventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	if (!bMouseWithInCanvas) return;
	UnhighlightSlots(LastHighlightedIndex,LastHighlightedDimensions);
	UInv_InventoryStatics::ForEach2D(GridSlotsArray, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetOccupiedTexture();
	});
	LastHighlightedDimensions = Dimensions;
	LastHighlightedIndex = Index;
}

void UInv_InventoryGrid::UnhighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	UInv_InventoryStatics::ForEach2D(GridSlotsArray, Index, Dimensions, Columns, [&](UInv_GridSlot* GridSlot)
	{
		if (GridSlot->GetGridSlotAvailable()) // if there is no item in the slot
		{
			GridSlot->SetUnoccupiedTexture();
		}
		else // if there is an item in the slot (remain highlighted)
		{
			GridSlot->SetOccupiedTexture();
		}
	});
}

void UInv_InventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimensions,
	EInv_GridSlotState GridSlotState)
{
	UnhighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UInv_InventoryStatics::ForEach2D(GridSlotsArray, Index, Dimensions, Columns, [State = GridSlotState](UInv_GridSlot* GridSlot)
	{
		switch (State)
		{
		case EInv_GridSlotState::Occupied:
			GridSlot->SetOccupiedTexture();
			break;
		case EInv_GridSlotState::Unoccupied:
			GridSlot->SetUnoccupiedTexture();
			break;
		case EInv_GridSlotState::Selected:
			GridSlot->SetSelectedTexture();
			break;
		case EInv_GridSlotState::GrayedOut:
			GridSlot->SetGrayedOutTexture();
			break;
		}
	});
	LastHighlightedIndex = Index;
	LastHighlightedDimensions = Dimensions;
}

FIntPoint UInv_InventoryGrid::CalculateStartingCoordinate(const FIntPoint& CurrentTileCoordinate, const FIntPoint& ItemDimensions , const EInv_TileQuadrant TileQuadrant) const
{
	const int32 HasEvenWidth = ItemDimensions.X % 2 == 0 ? 1 : 0;
	const int32 HasEvenHeight = ItemDimensions.Y % 2 == 0 ? 1 : 0;
	
	FIntPoint StartingCord;
	switch (TileQuadrant)
	{
	case EInv_TileQuadrant::TopLeft:
		StartingCord.X = CurrentTileCoordinate.X - FMath::FloorToInt(0.5 * ItemDimensions.X);
		StartingCord.Y = CurrentTileCoordinate.Y - FMath::FloorToInt(0.5 * ItemDimensions.Y);
		break;
		
	case EInv_TileQuadrant::TopRight:
		StartingCord.X = CurrentTileCoordinate.X - FMath::FloorToInt(0.5 * ItemDimensions.X) + HasEvenWidth;
		StartingCord.Y = CurrentTileCoordinate.Y - FMath::FloorToInt(0.5 * ItemDimensions.Y);
		break;
		
	case EInv_TileQuadrant::BottomLeft:
		StartingCord.X = CurrentTileCoordinate.X - FMath::FloorToInt(0.5 * ItemDimensions.X);
		StartingCord.Y = CurrentTileCoordinate.Y - FMath::FloorToInt(0.5 * ItemDimensions.Y) + HasEvenHeight;
		break;
		
	case EInv_TileQuadrant::BottomRight:
		StartingCord.X = CurrentTileCoordinate.X - FMath::FloorToInt(0.5 * ItemDimensions.X) + HasEvenWidth;
		StartingCord.Y = CurrentTileCoordinate.Y - FMath::FloorToInt(0.5 * ItemDimensions.Y) + HasEvenHeight;
		break;
		
	default: 
		UE_LOG(LogInventory, Error, TEXT("Invalid Quadrant"))
		return FIntPoint(-1,-1);
	}
	return StartingCord;
}

FIntPoint UInv_InventoryGrid::CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	return FIntPoint {
		static_cast<int32>(FMath::FloorToInt((MousePosition.X - CanvasPosition.X) / TileSize)),
		static_cast<int32>(FMath::FloorToInt((MousePosition.Y - CanvasPosition.Y) / TileSize))
	};
}

EInv_TileQuadrant UInv_InventoryGrid::CalculateTileQuadrant(const FVector2D& CanvasPosition,
	const FVector2D& MousePosition) const
{
	// Calculate relative position within the current tile
	const float TileLocalX = FMath::Fmod((MousePosition.X - CanvasPosition.X) , TileSize);
	const float TileLocalY = FMath::Fmod((MousePosition.Y - CanvasPosition.Y) , TileSize);
	
	// Determine which quadrant a mouse is in
	const bool bIsTop = TileLocalY < TileSize / 2; // Top if Y is in the upper half
	const bool bIsLeft = TileLocalX < TileSize / 2; // Left if x is in the left half
	
	EInv_TileQuadrant HoveredTileQuadrant{EInv_TileQuadrant::None};
	if (bIsTop && bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::TopLeft;
	else if (!bIsTop && bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::BottomLeft;
	else if (bIsTop && !bIsLeft) HoveredTileQuadrant =  EInv_TileQuadrant::TopRight;
	else if (!bIsTop && !bIsLeft) HoveredTileQuadrant = EInv_TileQuadrant::BottomRight;
	
	return HoveredTileQuadrant;
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
		return;
	}
	
	if (IsRightClicked(MouseEvent))
	{
		CreateItemPopUp(GridIndex);
		return;
	}
	
	// do the hover item and the clicked inventory item share the same type and stackable? (Stacking or combining cases)
	if (IsSameStackable(ClickedInventoryItem))
	{
		const int32 ClickedStackCount = GridSlotsArray[GridIndex]->GetStackCount();
		const FInv_StackableFragment* StackableFragment = ClickedInventoryItem->GetItemManifest().GetFragmentOfType<FInv_StackableFragment>();
		const int32 MaxStackSize = StackableFragment->GetMaxStackSize();
		const int32 RoomInClickedSlot = MaxStackSize - ClickedStackCount;
		const int32 HoveredStackCount = HoverItem->GetStackCount();
		
		// should we swap their stack count? (Room In Clicked Slot == 0 && HoverStackCount < Max Stack Size)
		if (ShouldSwapStackCounts(RoomInClickedSlot, HoveredStackCount,MaxStackSize))
		{
			SwapStackCounts(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}
		
		// should we consume the hover item's stacks? (Room in clicked slot >= Hovered stack count)
		if (ShouldConsumeHoverItemStacks(HoveredStackCount,RoomInClickedSlot))
		{
			ConsumeHoverItemStacks(ClickedStackCount,HoveredStackCount, GridIndex);
			return;
		}
		
		// should we fill in the stacks of the clicked item? (and not consume the hover item)
		if (ShouldFillInStack(RoomInClickedSlot,HoveredStackCount))
		{
			FillInStack(RoomInClickedSlot, HoveredStackCount - RoomInClickedSlot, GridIndex);
			return;
		}
		
		// is there no room in the clicked slot? (Do Nothing)
		if (RoomInClickedSlot == 0)
		{
			return;
		}
	}
	
	// swap with the hover item? (Swapping Case)
	SwapWithHoverItem(ClickedInventoryItem, GridIndex);
}

void UInv_InventoryGrid::CreateItemPopUp(int32 GridIndex)
{
	UInv_InventoryItem* RightClickedItem = GridSlotsArray[GridIndex]->GetInventoryItem().Get();
	
	if (!IsValid(RightClickedItem)) return;
	if (IsValid(GridSlotsArray[GridIndex]->GetItemPopUp())) return;
	
	ItemPopUp = CreateWidget<UInv_ItemPopUp>(this, ItemPopUpClass);
	GridSlotsArray[GridIndex]->SetItemPopUp(ItemPopUp);
	
	OwningCanvasPanel->AddChild(ItemPopUp);
	UCanvasPanelSlot* CanvasPanelSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(ItemPopUp);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	CanvasPanelSlot->SetPosition(MousePosition - ItemPopUpOffset);
	CanvasPanelSlot->SetSize(ItemPopUp->GetBoxSize());
	
	/*Bind callback functions fired if bound from Inv_ItemPopUp*/
	const int32 SliderMax = GridSlotsArray[GridIndex]->GetStackCount() - 1;
	if (RightClickedItem->IsStackable() && SliderMax > 0)
	{
		ItemPopUp->OnSplit.BindDynamic(this, &ThisClass::UInv_InventoryGrid::OnPopUpMenuSplit);
		ItemPopUp->SetSliderParams(SliderMax, FMath::Max(1, GridSlotsArray[GridIndex]->GetStackCount()/2));
	}
	else
	{
		ItemPopUp->CollapseSplitButton();
	}
	
	ItemPopUp->OnDrop.BindDynamic(this, &ThisClass::OnPopUpMenuDrop);
	
	if (RightClickedItem->IsConsumable())
	{
		ItemPopUp->OnConsume.BindDynamic(this, &ThisClass::UInv_InventoryGrid::OnPopUpMenuConsume);
	}
	else
	{
		ItemPopUp->CollapseConsumeButton();
	}
}

void UInv_InventoryGrid::SwapWithHoverItem(UInv_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	if (!IsValid(HoverItem)) return;
	
	UInv_InventoryItem* TempInventoryItem = HoverItem->GetInventoryItem();
	const int32 TempStackCount = HoverItem->GetStackCount();
	const bool bTempIsStackable = HoverItem->IsStackable();
	
	// keep the same previous grid index
	AssignHoverItem(ClickedInventoryItem, GridIndex, HoverItem->GetPreviousGridIndex());
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
	AddItemAtIndex(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
	UpdateGridSlots(TempInventoryItem,ItemDropIndex,bTempIsStackable, TempStackCount);
}

bool UInv_InventoryGrid::ShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoverStackCount,
	const int32 MaxStackSize) const
{
	return RoomInClickedSlot == 0 && HoverStackCount < MaxStackSize;
}

void UInv_InventoryGrid::SwapStackCounts(const int32 ClickedStackCount, const int32 HoveredStackCount,
	const int32 Index)
{
	UInv_GridSlot* GridSlot = GridSlotsArray[Index];
	GridSlot->SetStackCount(HoveredStackCount);
	
	UInv_SlottedItem* ClickedSlottedItem = SlottedItemsMap.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(HoveredStackCount);
	
	HoverItem->UpdateStackCount(ClickedStackCount);
}

bool UInv_InventoryGrid::ShouldConsumeHoverItemStacks(const int32 HoveredStackCount,
	const int32 RoomInClickedSlot) const
{
	return RoomInClickedSlot >= HoveredStackCount;
}

void UInv_InventoryGrid::ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoveredStackCount,
	const int32 Index)
{
	const int32 AmountToTransfer = HoveredStackCount;
	const int32 NewClickedStackCount = ClickedStackCount + AmountToTransfer;
	GridSlotsArray[Index]->SetStackCount(NewClickedStackCount);
	SlottedItemsMap.FindChecked(Index)->UpdateStackCount(NewClickedStackCount);
	ClearHoverItem();
	ShowCursor();
	const FInv_GridFragment* GridFragment = GridSlotsArray[Index]->GetInventoryItem()->GetItemManifest().GetFragmentOfType<FInv_GridFragment>();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1,1);
	HighlightSlots(Index,Dimensions);
	
}

bool UInv_InventoryGrid::ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const
{
	return RoomInClickedSlot < HoveredStackCount;
}

void UInv_InventoryGrid::FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index)
{
	UInv_GridSlot* GridSlot = GridSlotsArray[Index];
	const int32 NewStackCount = GridSlot->GetStackCount() + FillAmount;
	GridSlot->SetStackCount(NewStackCount);
	
	UInv_SlottedItem* ClickedSlottedItem = SlottedItemsMap.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(NewStackCount);
	
	if (HoverItem) HoverItem->UpdateStackCount(Remainder);
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
	AssignHoverItem(ClickedInventoryItem, GridIndex, GridIndex);
	
	// Remove clicked item, from the grid
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
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

void UInv_InventoryGrid::AssignHoverItem(UInv_InventoryItem* InventoryItem, const int32 GridIndex,
	const int32 PreviousGridIndex)
{
	AssignHoverItem(InventoryItem);
	HoverItem->SetPreviousGridIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ? GridSlotsArray[GridIndex]->GetStackCount() : 0);
}

void UInv_InventoryGrid::RemoveItemFromGrid(const UInv_InventoryItem* InventoryItem, const int32 GridIndex)
{
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(InventoryItem, FragmentTags::GridFragment);
	if (!GridFragment) return;
	
	UInv_InventoryStatics::ForEach2D(GridSlotsArray, GridIndex, GridFragment->GetGridSize(), Columns, [&](UInv_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(nullptr);
		GridSlot->SetUpperLeftIndex(INDEX_NONE);
		GridSlot->SetUnoccupiedTexture();
		GridSlot->SetGridSlotAvailable(true);
		GridSlot->SetStackCount(0);
	});
	
	if (SlottedItemsMap.Contains(GridIndex))
	{
		TObjectPtr<UInv_SlottedItem> FoundSlottedItem;	
		SlottedItemsMap.RemoveAndCopyValue(GridIndex,FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
}

void UInv_InventoryGrid::SetOwningCanvas(UCanvasPanel* OwningCanvas)
{
	OwningCanvasPanel = OwningCanvas;
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
		
		// how much is the remainder?
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
			/*Bind to the events fired from Inv_GridSlot*/
			GridSlot->GridSlotClicked.AddDynamic(this, &ThisClass::UInv_InventoryGrid::OnGridSlotClicked);
			GridSlot->GridSlotHovered.AddDynamic(this, &ThisClass::UInv_InventoryGrid::OnGridSlotHovered);
			GridSlot->GridSlotUnhovered.AddDynamic(this, &ThisClass::UInv_InventoryGrid::OnGridSlotUnhovered);
		}
	}
}

void UInv_InventoryGrid::OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (!IsValid(HoverItem)) return;
	if (!GridSlotsArray.IsValidIndex(ItemDropIndex)) return;
	
	if (CurrentQueryResult.ValidInventoryItem.IsValid() && GridSlotsArray.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex, MouseEvent);
		return;
	}
	auto GridSlot = GridSlotsArray[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid())
	{
		PutDownOnIndex(ItemDropIndex);
	}
}

void UInv_InventoryGrid::PutDownOnIndex(const int32 Index)
{
	AddItemAtIndex(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	UpdateGridSlots(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	ClearHoverItem();
}

void UInv_InventoryGrid::ClearHoverItem()
{
	if (!IsValid(HoverItem)) return;
	HoverItem->SetInventoryItem(nullptr);
	HoverItem->SetIsStackable(false);
	HoverItem->SetPreviousGridIndex(INDEX_NONE);
	HoverItem->UpdateStackCount(0);
	
	HoverItem->SetImageBruh(FSlateNoResource());
	HoverItem->RemoveFromParent();
	HoverItem = nullptr;
	
	ShowCursor();
}

UUserWidget* UInv_InventoryGrid::GetVisibleCursorWidget() /*Lazy Loading Functions*/
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(VisibleCursorWidget))
	{
		VisibleCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), VisibleCursorWidgetClass);
	}
	return VisibleCursorWidget;
}

UUserWidget* UInv_InventoryGrid::GetHiddenCursorWidget() /*Lazy Loading Functions*/
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(HiddenCursorWidget))
	{
		HiddenCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), HiddenCursorWidgetClass);
	}
	return HiddenCursorWidget;
}

bool UInv_InventoryGrid::IsSameStackable(const UInv_InventoryItem* ClickedInventoryItem) const
{
	const bool bIsSameItem = ClickedInventoryItem == HoverItem->GetInventoryItem();
	const bool bIsStackable = ClickedInventoryItem->IsStackable();
	return bIsSameItem && bIsStackable && HoverItem->GetItemType().MatchesTagExact(ClickedInventoryItem->GetItemManifest().GetItemType());
}

void UInv_InventoryGrid::ShowCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetVisibleCursorWidget());
}

void UInv_InventoryGrid::HideCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetHiddenCursorWidget());
}

void UInv_InventoryGrid::OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;
	UInv_GridSlot* GridSlot = GridSlotsArray[GridIndex];
	if (GridSlot->GetGridSlotAvailable())
	{
		GridSlot->SetOccupiedTexture();
	}
}

void UInv_InventoryGrid::OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;
	UInv_GridSlot* GridSlot = GridSlotsArray[GridIndex];
	if (GridSlot->GetGridSlotAvailable())
	{
		GridSlot->SetUnoccupiedTexture();
	}
}

void UInv_InventoryGrid::OnPopUpMenuSplit(int32 SplitAmount, int32 Index)
{
	UInv_InventoryItem* RightClickedInventoryItem = GridSlotsArray[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedInventoryItem)) return;
	if (!RightClickedInventoryItem->IsStackable()) return;
	
	const int32 UpperLeftIndex = GridSlotsArray[Index]->GetUpperLeftIndex();
	UInv_GridSlot* UpperLeftGridSlot = GridSlotsArray[UpperLeftIndex];
	const int32 StackCount = UpperLeftGridSlot->GetStackCount();
	const int32 NewStackCount = StackCount - SplitAmount;
	
	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItemsMap.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);
	
	AssignHoverItem(RightClickedInventoryItem, UpperLeftIndex, UpperLeftIndex);
	HoverItem->UpdateStackCount(SplitAmount);
}

void UInv_InventoryGrid::OnPopUpMenuDrop(int32 Index)
{
	UInv_InventoryItem* RightClickedInventoryItem = GridSlotsArray[Index]->GetInventoryItem().Get();
	if (!IsValid(RightClickedInventoryItem)) return;
	
	PickUp(RightClickedInventoryItem,Index);
	
	DropItem();
}

void UInv_InventoryGrid::DropItem()
{
	if (!IsValid(HoverItem)) return;
	if (!IsValid(HoverItem->GetInventoryItem())) return;
	
	InventoryComponent->Server_DropItem(HoverItem->GetInventoryItem(),HoverItem->GetStackCount());
	
	ClearHoverItem();
	ShowCursor();
}

void UInv_InventoryGrid::OnPopUpMenuConsume(int32 Index)
{
	
}