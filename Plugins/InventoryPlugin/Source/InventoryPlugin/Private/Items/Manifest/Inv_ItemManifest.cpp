#include "Items/Manifest/Inv_ItemManifest.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"

UInv_InventoryItem* FInv_ItemManifest::ManifestCreation(UObject* NewOuter) const
{
	UInv_InventoryItem* InventoryItem = NewObject<UInv_InventoryItem>(NewOuter,UInv_InventoryItem::StaticClass());
	InventoryItem->SetItemManifest(*this);
	
	return InventoryItem;
}

void FInv_ItemManifest::SpawnPickUpActor(const UObject* WorldContextObject, const FVector& SpawnLocation,
	const FRotator& SpawnRotation) const
{
	if (!IsValid(PickUpActorClass)) return; 
	if (!IsValid(WorldContextObject)) return;
	
	AActor* SpawnedActor =  WorldContextObject->GetWorld()->SpawnActor<AActor>(PickUpActorClass,SpawnLocation, SpawnRotation);
	
	if (!IsValid(SpawnedActor)) return;
	
	UInv_ItemComponent* ItemComp = SpawnedActor->FindComponentByClass<UInv_ItemComponent>();
	check(ItemComp);
	
	ItemComp->InitItemManifest(*this);
	
}