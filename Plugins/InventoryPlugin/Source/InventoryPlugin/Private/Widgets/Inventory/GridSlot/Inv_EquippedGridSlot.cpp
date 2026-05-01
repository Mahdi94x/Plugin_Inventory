// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Inventory/GridSlot/Inv_EquippedGridSlot.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "InventoryManagment/Utils/Inv_InventoryStatics.h"
#include "Widgets/Inventory/HoverItem/Inv_HoverItem.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Fragments/Inv_FragmentTags.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Inventory/SlottedItems/Inv_EquippedSlottedItem.h"

UInv_EquippedSlottedItem* UInv_EquippedGridSlot::OnItemEquipped(UInv_InventoryItem* Item,
                                                                const FGameplayTag& EquipmentTag, float TileSize)
{
	// Check the equipment type tag
	if (!EquipmentTag.MatchesTagExact(EquipmentTypeTag)) return nullptr;
	
	// get grid dimensions
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(Item, FragmentTags::GridFragment);
	if (!GridFragment) return nullptr;
	const FIntPoint GridDimension = GridFragment->GetGridSize();
	
	// calculate the draw size for the equipped slotted item
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding()*2;
	const FVector2D DrawSize = GridDimension * IconTileWidth;
	
	// create the equipped slotted item
	EquippedSlottedItem = CreateWidget<UInv_EquippedSlottedItem>(GetOwningPlayer(), EquippedSlottedItemClass);
	
	// set the slotted item inventory item - equipment type tag - hide the stack count on the slotted item
	EquippedSlottedItem->SetInventoryItem(Item);
	EquippedSlottedItem->SetEquipmentTypeTag(EquipmentTag);
	EquippedSlottedItem->UpdateStackCount(0);
	
	// set inventory item on this class the equipped grid slot
	this->SetInventoryItem(Item);
	
	// set the image brush on the equipped slotted item
	const FInv_IconFragment* IconFragment = GetFragment<FInv_IconFragment>(Item, FragmentTags::IconFragment);
	if (!IconFragment) return nullptr;
	FSlateBrush Brush;
	Brush.SetResourceObject(IconFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = DrawSize;
	EquippedSlottedItem->SetImageBrush(Brush);
	
	// add the equipped slotted item as a child to this widget (equipped grid slot) overlay
	Overlay->AddChildToOverlay(EquippedSlottedItem);
	FGeometry OverlayGeometry = Overlay->GetCachedGeometry();
	auto OverlayPos = OverlayGeometry.Position;
	auto OverlaySize = OverlayGeometry.Size;
	const float LeftPadding = OverlaySize.X /2.f - DrawSize.X / 2.f;
	const float RightPadding =  OverlaySize.Y /2.f - DrawSize.Y / 2.f;
	UOverlaySlot* OverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(EquippedSlottedItem);
	OverlaySlot->SetPadding(FMargin(LeftPadding,RightPadding));
	
	// return the UInv_EquippedSlottedItem created
	return EquippedSlottedItem;
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
