#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Inv_FastArray.generated.h"

struct FGameplayTag;
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
	
	friend struct FInv_InventoryFastArray;
	friend UInv_InventoryComponent;
	UPROPERTY()
	TObjectPtr<UInv_InventoryItem> Item = nullptr;
};

/** A list of inventory item (only replicates deltas) */
USTRUCT(BlueprintType)
struct FInv_InventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()
	
	FInv_InventoryFastArray() : OwnerComponent(nullptr) {}
	FInv_InventoryFastArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}
	
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
	UInv_InventoryItem* FindFirstItemByType(const FGameplayTag& ItemType);

private:
	
	friend UInv_InventoryComponent;
	
	// Replicated List Of inventory items
	UPROPERTY()
	TArray<FInv_InventoryEntry> Entries;
	
	// Owner Component
	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<> /* Set a flag that this type (FInv_InventoryFastArray) should be delta serialized*/
struct TStructOpsTypeTraits<FInv_InventoryFastArray> : TStructOpsTypeTraitsBase2<FInv_InventoryFastArray>
{
	enum {WithNetDeltaSerializer = true};
};
