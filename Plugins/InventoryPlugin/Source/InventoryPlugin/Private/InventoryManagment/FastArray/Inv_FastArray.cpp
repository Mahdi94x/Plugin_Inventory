#include "InventoryManagment/FastArray/Inv_FastArray.h"
#include"Items/Inv_InventoryItem.h"
#include "InventoryManagment/Components/Inv_InventoryComponent.h"
#include "Items/Components/Inv_ItemComponent.h"

TArray<UInv_InventoryItem*> FInv_InventoryFasTArray::GetAllItems() const
{
	TArray<UInv_InventoryItem*> Results;
	Results.Reserve(Entries.Num());
	for (const auto& Entry : Entries)
	{
		if (!IsValid(Entry.Item)) continue;
		Results.Add(Entry.Item);
	}
	return Results;
}

void FInv_InventoryFasTArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	UInv_InventoryComponent* InventoryComponent = Cast<UInv_InventoryComponent>(OwnerComponent);
	if (!IsValid(InventoryComponent)) return;
	
	for (const auto& Index : RemovedIndices)
	{
		InventoryComponent->OnItemRemoved.Broadcast(Entries[Index].Item);
	}
}

void FInv_InventoryFasTArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	UInv_InventoryComponent* InventoryComponent = Cast<UInv_InventoryComponent>(OwnerComponent);
	if (!IsValid(InventoryComponent)) return;
	
	for (const auto& Index : AddedIndices)
	{
		InventoryComponent->OnItemAdded.Broadcast(Entries[Index].Item);
	}
	
}

bool FInv_InventoryFasTArray::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FastArrayDeltaSerialize<FInv_InventoryEntry, FInv_InventoryFasTArray>(Entries, DeltaParams,*this);
}

UInv_InventoryItem* FInv_InventoryFasTArray::AddEntry(UInv_ItemComponent* ItemComponent)
{
	check(OwnerComponent); /*Inv_InventoryComponent*/ 
	AActor* OwningActor = OwnerComponent->GetOwner(); /*Inv_PlayerController*/
	check(OwningActor->HasAuthority());
	
	UInv_InventoryComponent* InventoryComponent = Cast<UInv_InventoryComponent>(OwnerComponent);
	if (!IsValid(InventoryComponent)) return nullptr;
	
	FInv_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = ItemComponent->GetItemComponent_Manifest().ManifestCreation(OwningActor);
	/*NewEntry.Item is a UObject must be replicated as a subobject in Inv_InventoryComponent*/
	
	InventoryComponent->AddRepSubObj(NewEntry.Item);
	MarkItemDirty(NewEntry);
	
	return NewEntry.Item;
	
}

UInv_InventoryItem* FInv_InventoryFasTArray::AddEntry(UInv_InventoryItem* InventoryItem) /*From FAS Header file*/
{
	check(OwnerComponent); /*Inv_InventoryComponent*/ 
	const AActor* OwningActor = OwnerComponent->GetOwner(); /*Inv_PlayerController*/
	check(OwningActor->HasAuthority());
	
	FInv_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = InventoryItem;
	
	MarkItemDirty(NewEntry); 
	return InventoryItem;
}

void FInv_InventoryFasTArray::RemoveEntry(UInv_InventoryItem* InventoryItem) /*From FAS Header file*/
{
	for (auto EntryIterator = Entries.CreateIterator(); EntryIterator; ++EntryIterator)
	{
		FInv_InventoryEntry& Entry = *EntryIterator;
		if (Entry.Item == InventoryItem)
		{
			EntryIterator.RemoveCurrent();
		}
	}
	MarkArrayDirty();
}
