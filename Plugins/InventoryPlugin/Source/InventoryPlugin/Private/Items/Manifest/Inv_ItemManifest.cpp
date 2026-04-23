#include "Items/Manifest/Inv_ItemManifest.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Composite/Inv_CompositeBase.h"

UInv_InventoryItem* FInv_ItemManifest::ManifestCreation(UObject* NewOuter) 
{
	UInv_InventoryItem* InventoryItem = NewObject<UInv_InventoryItem>(NewOuter, UInv_InventoryItem::StaticClass());

	// Deep copy the manifest (including fragments) from the pickup actor (template)
	InventoryItem->SetItemManifest(*this);

	// Initialize the copied fragments (e.g., randomize values)
	for (auto& Fragment : InventoryItem->GetItemManifestMutable().ItemFragments)
	{
		Fragment.GetMutable().FragmentManifest();
	}

	/* Clear fragments from the source manifest (pickup actor),
	* AFTER copying, to prevent shared state, duplication, or stale data.
	* This does NOT affect the inventory item's fragments.*/
	ClearFragments();

	return InventoryItem;
}

void FInv_ItemManifest::ClearFragments()
{
	for (auto& Fragment:ItemFragments)
	{
		Fragment.Reset();
	}
	ItemFragments.Empty();
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

void FInv_ItemManifest::AssimilateInventoryFragments(UInv_CompositeBase* Composite) const
{
	const auto& InventoryItemFragments = GetAllFragmentOfType<FInv_InventoryItemFragment>();
	for (const auto& Fragment : InventoryItemFragments)
	{
		Composite->ApplyFunction([Fragment](UInv_CompositeBase* Widget)
		{
			Fragment->Assimilate(Widget);
		});
	}
}
