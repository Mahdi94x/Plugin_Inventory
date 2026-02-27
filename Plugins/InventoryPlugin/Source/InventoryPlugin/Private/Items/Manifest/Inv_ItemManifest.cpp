
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Items/Inv_InventoryItem.h"

UInv_InventoryItem* FInv_ItemManifest::ManifestCreation(UObject* NewOuter)
{
	UInv_InventoryItem* InventoryItem = NewObject<UInv_InventoryItem>(NewOuter,UInv_InventoryItem::StaticClass());
	InventoryItem->SetItemManifest(*this);
	
	return InventoryItem;
}
