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

void UInv_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ConstructGridSlots();
	
	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &UInv_InventoryGrid::AddItem);
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
	Result.TotalRoomToFill = 1;
	
	FInv_SlotAvailability SlotAvailability;
	SlotAvailability.AmountToFill = 1;
	SlotAvailability.Index = 0;
	
	Result.SlotAvailabilities.Add(MoveTemp(SlotAvailability)); /*move semantics for optimization*/
	
	return Result;
}

void UInv_InventoryGrid::AddItem(UInv_InventoryItem* InventoryItem)
{
	if (!MatchesCategory(InventoryItem)) return;
	
	const FInv_SlotAvailabilityResult Result = HasRoomForItem_Grid_II(InventoryItem);
	AddItemToIndices(Result, InventoryItem);
}

void UInv_InventoryGrid::AddItemToIndices(const FInv_SlotAvailabilityResult& Result,
	const UInv_InventoryItem* NewItem)
{
	// get grid fragment from the item using gameplay tags so we know how many grid spaces the item take
	const FInv_GridFragment* GridFragment = GetFragment<FInv_GridFragment>(NewItem, FragmentTags::GridFragment);
	
	// Get image fragment so we have the icon to display
	const FInv_ImageFragment* ImageFragment = GetFragment<FInv_ImageFragment>(NewItem, FragmentTags::IconFragment);
	
	if (!GridFragment || !ImageFragment) return;
	
	// Create a widget to add to the grid
	// Store the created widget in a container for future use
	
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

bool UInv_InventoryGrid::MatchesCategory(const UInv_InventoryItem* InventoryItem) const
{
	return this->ItemCategory == InventoryItem->GetItemManifest().GetItemCategory();
}





