// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Inventory/GridSlot/Inv_EquippedGridSlot.h"
#include "InventoryManagment/Utils/Inv_InventoryStatics.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Components/Image.h"

UInv_EquippedSlottedItem* UInv_EquippedGridSlot::OnItemEqiupped(UInv_InventoryItem* Item,
	const FGameplayTag& EquipmentTag, float TileSize)
{
	// Check the equipment type tag
	
	// get grid dimensions
	
	// calculate the draw size for the equipped slotted item
	
	// create the equipped slotted item
	
	// set the slotted item inventory item - equipment type tag - hide the stack count on the slotted item
	
	// set inventory item on this class the equipped grid slot
	
	// set the image brush on the equipped slotted item
	
	// add the equipped slotted item as a child to this widget (equipped grid slot) overlay
	
	// return the UInv_EquippedSlottedItem created
	return nullptr;
}

void UInv_EquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!GetGridSlotAvailable()) return;
	UInv_HoverItem* HoverItem = UInv_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;
	
	/*More Specific Tag compared to less specific tag*/
	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetOccupiedTexture();
		Image_GrayedOutImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UInv_EquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	if (!GetGridSlotAvailable()) return;
	UInv_HoverItem* HoverItem = UInv_InventoryStatics::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;
	
	/*More Specific Tag compared to less specific tag*/
	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		SetUnoccupiedTexture();
		Image_GrayedOutImage->SetVisibility(ESlateVisibility::Visible);
	}
}

FReply UInv_EquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	EquippedGridSlotClicked.Broadcast(this, EquipmentTypeTag);
	return FReply::Handled();
}
