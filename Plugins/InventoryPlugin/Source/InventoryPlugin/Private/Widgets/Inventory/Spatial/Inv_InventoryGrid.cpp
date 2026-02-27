// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Inventory/Spatial/Inv_InventoryGrid.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Widgets/Inventory/GridSlot/Inv_GridSlot.h"
#include "Components\CanvasPanel.h"
#include "InventoryManagment/Components/Inv_InventoryComponent.h"
#include "InventoryManagment/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Widgets/Utils/Inv_WidgetUtils.h"

void UInv_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	ConstructGridSlots();
	
	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(GetOwningPlayer());
	InventoryComponent->OnItemAdded.AddDynamic(this, &UInv_InventoryGrid::AddItem);
}

void UInv_InventoryGrid::AddItem(UInv_InventoryItem* InventoryItem)
{
	if (!MatchesCategory(InventoryItem)) return;
	
	UE_LOG(LogTemp, Warning, TEXT("UInv_InventoryGrid::AddItem()"));
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

