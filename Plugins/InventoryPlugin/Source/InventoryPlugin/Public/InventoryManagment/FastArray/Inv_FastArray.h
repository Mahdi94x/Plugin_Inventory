#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Inv_FastArray.generated.h"

class UInv_ItemComponent;
class UInv_InventoryComponent;
class UInv_InventoryItem;

 /** A single entry in an Inventory */
USTRUCT(BlueprintType)
struct FInv_InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	FInv_InventoryEntry() {}
	
private:
	
	friend struct FInv_InventoryFasTArray;
	friend UInv_InventoryComponent;
	UPROPERTY()
	TObjectPtr<UInv_InventoryItem> Item = nullptr;
};

/** A list of inventory item (only replicated deltas) */
USTRUCT(BlueprintType)
struct FInv_InventoryFasTArray : public FFastArraySerializer
{
	GENERATED_BODY()
	
	FInv_InventoryFasTArray() : OwnerComponent(nullptr) {}
	FInv_InventoryFasTArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}
	
	// FFastArraySerializer contract, Replication Event Hooks
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	// End FFastArraySerializer contract, Replication Event Hooks
	
	/*Handles Delta Serialization*/
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams);
	
	/*Additional Utility Functions*/
	TArray<UInv_InventoryItem*> GetAllItems() const;
	UInv_InventoryItem* AddEntry(UInv_ItemComponent* ItemComponent);
	UInv_InventoryItem* AddEntry(UInv_InventoryItem* InventoryItem);
	void RemoveEntry(UInv_InventoryItem* InventoryItem);

private:
	
	friend UInv_InventoryComponent;
	
	// Replicated List Of inventory items
	UPROPERTY()
	TArray<FInv_InventoryEntry> Entries;
	
	// Owner Component
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<> /* Set a flag that this type (FInv_InventoryFasTArray) should be delta serialized*/
struct TStructOpsTypeTraits<FInv_InventoryFasTArray> : TStructOpsTypeTraitsBase2<FInv_InventoryFasTArray>
{
	enum {WithNetDeltaSerializer = true};
};
