#pragma once

#include "Inv_GridTypes.generated.h"

class FInv_InventoryItem;

UENUM(BlueprintType)
enum class EInv_ItemCategory : uint8
{
	Equippable,
	Consumable,
	Craftable,
	None
};

USTRUCT()
struct FInv_SlotAvailability /** Results of an individual slot, we will have an array of these*/
{
	GENERATED_BODY()
	
	FInv_SlotAvailability() {}
	FInv_SlotAvailability(int32 ItemIndex, int32 Room, bool bHasItem) : 
		Index(ItemIndex), AmountToFill(Room), bItemAtIndex(bHasItem) {}
	
	int32 Index{INDEX_NONE}; /**The Slot Index in the grid*/
	int32 AmountToFill{0}; /**How much of the item goes in this slot*/
	bool bItemAtIndex{false}; /**If True increase stack, if false or there is a remainder, create a new widget*/
};

USTRUCT()
struct FInv_SlotAvailabilityResult /** Results of the slots in the entire grid*/
{
	GENERATED_BODY()
	
	FInv_SlotAvailabilityResult() {}
	
	TWeakObjectPtr<FInv_InventoryItem> Item; /**Weak pointer to the item if it exists*/
	bool bStackable{false}; /**Is this item stackable or not?*/
	int32 TotalRoomToFill{0}; /**Number of items that can fit in the grid, if it equals 0, there is no room*/
	int32 Remainder{0};/**Number of items that can't fit*/
	TArray<FInv_SlotAvailability> SlotAvailabilities; /**List of all the slots that needs updating*/
	
};