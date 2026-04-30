// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Inventory/Spatial/Inv_SpatialInventory.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "InventoryManagment/Utils/Inv_InventoryStatics.h"
#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"
#include "InventoryPlugin.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Widgets/ItemDescription/Inv_ItemDescription.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Items/Inv_InventoryItem.h"
#include "Blueprint/WidgetTree.h"
#include "Widgets/Inventory/GridSlot/Inv_EquippedGridSlot.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"

void UInv_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Button_Equippables->OnClicked.AddDynamic(this, &ThisClass::ShowEquippables);
	Button_Consumables->OnClicked.AddDynamic(this, &ThisClass::ShowConsumables);
	Button_Craftables->OnClicked.AddDynamic(this, &ThisClass::ShowCraftables);
	
	Grid_Equippables->SetOwningCanvas(CanvasPanel);
	Grid_Consumables->SetOwningCanvas(CanvasPanel);
	Grid_Craftables->SetOwningCanvas(CanvasPanel);
	
	ShowEquippables();
	
	WidgetTree->ForEachWidget([this](UWidget* Widget)
	{
		if (UInv_EquippedGridSlot* EquippedGridSlot = Cast<UInv_EquippedGridSlot>(Widget); IsValid(EquippedGridSlot))
		{
			EquippedGridSlotArray.Add(EquippedGridSlot);
			EquippedGridSlot->EquippedGridSlotClicked.AddDynamic(this, &ThisClass::EquippedGridSlotClicked);
		}
	});
}

void UInv_SpatialInventory::EquippedGridSlotClicked(UInv_EquippedGridSlot* EquippedGridSlot,
	const FGameplayTag& EquipmentTypeTag)
{
	// Check to see if we can equip the hover item
	if (!CanEquipHoverItem(EquippedGridSlot, EquipmentTypeTag)) return;
	
	// create an equipped slotted item and add it to the equipped grid slot (EquippedGridSlot->OnItemEqiupped (3 params))
	
	// clear the hover item
	
	// inform the server that an item has been equipped (also unequipping an existing item "Swapping")
}

bool UInv_SpatialInventory::CanEquipHoverItem(UInv_EquippedGridSlot* EquippedGridSlot,
	const FGameplayTag& EquipmentTypeTag) const
{
	if (!IsValid(EquippedGridSlot) || EquippedGridSlot->GetInventoryItem().IsValid()) return false;
	
	UInv_HoverItem* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem)) return false;
	
	UInv_InventoryItem* HeldItem = HoverItem->GetInventoryItem();
	
	return HasHoverItem() &&
		IsValid(HeldItem) &&
			!HoverItem->IsStackable() &&
				HeldItem->GetItemManifest().GetItemCategory() == EInv_ItemCategory::Equippable &&
					HeldItem->GetItemManifest().GetItemType().MatchesTag(EquipmentTypeTag);
}

FReply UInv_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	ActiveGrid->DropItem();
	return FReply::Handled();
}

FInv_SlotAvailabilityResult UInv_SpatialInventory::HasRoomForItem(UInv_ItemComponent* ItemComponent) const
{
	switch (UInv_InventoryStatics::GetItemCategoryFromItemComp(ItemComponent))
	{
	case EInv_ItemCategory::Equippable:
		return Grid_Equippables->HasRoomForItem_Grid_IC(ItemComponent);

	case EInv_ItemCategory::Consumable:
		return Grid_Consumables->HasRoomForItem_Grid_IC(ItemComponent);

	case EInv_ItemCategory::Craftable:
		return Grid_Craftables->HasRoomForItem_Grid_IC(ItemComponent);

	case EInv_ItemCategory::None:
		UE_LOG(LogInventory, Error, TEXT("ItemComponent doesn't have a valid item category"));
		return FInv_SlotAvailabilityResult();

	default:
		UE_LOG(LogInventory, Error, TEXT("Unhandled item category in HasRoomForItem"));
		return FInv_SlotAvailabilityResult();
	}
}

void UInv_SpatialInventory::OnItemHovered(UInv_InventoryItem* Item)
{
	const auto& Manifest = Item->GetItemManifest();
	UInv_ItemDescription* DescriptionWidget = GetItemDescription();
	
	DescriptionWidget->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
	
	FTimerDelegate DescriptionTimerDelegate;
	DescriptionTimerDelegate.BindLambda([this, &Manifest, DescriptionWidget]()
	{
		DescriptionWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
		Manifest.AssimilateInventoryFragments(DescriptionWidget);
	});
	
	GetOwningPlayer()->GetWorldTimerManager().SetTimer(DescriptionTimer,DescriptionTimerDelegate,DescriptionTimerDelay,false);
}

void UInv_SpatialInventory::OnItemUnhovered()
{
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	GetOwningPlayer()->GetWorldTimerManager().ClearTimer(DescriptionTimer);
}

UInv_ItemDescription* UInv_SpatialInventory::GetItemDescription()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription= CreateWidget<UInv_ItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		CanvasPanel->AddChild(ItemDescription);
	}
	return ItemDescription;
}

bool UInv_SpatialInventory::HasHoverItem() const
{
	if (Grid_Equippables->GridHasHoverItem()) return true;
	if (Grid_Consumables->GridHasHoverItem()) return true;
	if (Grid_Craftables->GridHasHoverItem()) return true;
	
	return false;
}

UInv_HoverItem* UInv_SpatialInventory::GetHoverItem() const
{
	if (!ActiveGrid.IsValid()) return nullptr;
	return ActiveGrid->GetHoverItem();
}

void UInv_SpatialInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!IsValid(ItemDescription)) return;
	SetItemDescriptionSizeAndPosition(ItemDescription, CanvasPanel);
}

void UInv_SpatialInventory::SetItemDescriptionSizeAndPosition(UInv_ItemDescription* Description,
	UCanvasPanel* Canvas) const
{
	UCanvasPanelSlot* ItemDescriptionCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(Description);
	if (!IsValid(ItemDescriptionCPS)) return;
	
	const FVector2D ItemDescriptionSize = Description->GetBoxSize();
	ItemDescriptionCPS->SetSize(ItemDescriptionSize);
	
	FVector2D ClampedPosition = UInv_WidgetUtils::GetClampedWidgetPosition(
		UInv_WidgetUtils::GetWidgetSize(Canvas), 
		ItemDescriptionSize, 
		UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer()));
	
	ItemDescriptionCPS->SetPosition(ClampedPosition);
}

void UInv_SpatialInventory::ShowEquippables()
{
	SetActiveGrid(Grid_Equippables, Button_Equippables);
}

void UInv_SpatialInventory::ShowConsumables()
{
	SetActiveGrid(Grid_Consumables, Button_Consumables);
}

void UInv_SpatialInventory::ShowCraftables()
{
	SetActiveGrid(Grid_Craftables, Button_Craftables);
}

void UInv_SpatialInventory::SetActiveGrid(UInv_InventoryGrid* Grid, UButton* Button) 
{
	if (ActiveGrid.IsValid()) ActiveGrid->HideCursor();
	ActiveGrid = Grid;
	if (ActiveGrid.IsValid()) ActiveGrid->ShowCursor();
	DisableButton(Button);
	Switcher->SetActiveWidget(Grid);
}

void UInv_SpatialInventory::DisableButton(UButton* Button) 
{
	Button_Equippables->SetIsEnabled(true);
	Button_Craftables->SetIsEnabled(true);
	Button_Consumables->SetIsEnabled(true);
	Button->SetIsEnabled(false);
}
