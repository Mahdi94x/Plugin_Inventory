#include "InventoryManagment/FastArray/Inv_FastArray.h"
#include"Items/Inv_InventoryItem.h"
#include "InventoryManagment/Components/Inv_InventoryComponent.h"

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
	return nullptr;
	// TODO: Implement once ItemComponent is complete (For now it only contain a message to display for pickup widget)
}

UInv_InventoryItem* FInv_InventoryFasTArray::AddEntry(UInv_InventoryItem* Item) /*From FAS Header file*/
{
	check(OwnerComponent);
	const AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());
	
	FInv_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = Item;
	
	MarkItemDirty(NewEntry); 
	return Item;
}

void FInv_InventoryFasTArray::RemoveEntry(UInv_InventoryItem* Item) /*From FAS Header file*/
{
	for (auto EntryIterator = Entries.CreateIterator(); EntryIterator; ++EntryIterator)
	{
		FInv_InventoryEntry& Entry = *EntryIterator;
		if (Entry.Item == Item)
		{
			EntryIterator.RemoveCurrent();
		}
	}
	MarkArrayDirty();
}
