#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/Inv_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "Inv_ItemManifest.generated.h"

/*
 * The Item Manifest contains the required data for creating a new Inv_InventoryItem
 */

class UInv_CompositeBase;
struct FInv_ItemFragmentBase;
class UInv_InventoryItem;

USTRUCT(BlueprintType)
struct INVENTORYPLUGIN_API FInv_ItemManifest
{
	GENERATED_BODY()
	
	EInv_ItemCategory GetItemCategory() const {return this->ItemCategory; }
	UInv_InventoryItem* ManifestCreation(UObject* NewOuter) const;
	FGameplayTag GetItemType() const {return this->ItemType;}
	
	template<typename T> requires std::derived_from<T, FInv_ItemFragmentBase>
	const T* GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const;
	
	template<typename T> requires std::derived_from<T, FInv_ItemFragmentBase>
	const T* GetFragmentOfType() const;
	
	template<typename T> requires std::derived_from<T, FInv_ItemFragmentBase>
	T* GetFragmentOfTypeMutable();
	
	void SpawnPickUpActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation) const;
	
	template<typename T> requires std::derived_from<T, FInv_ItemFragmentBase>
	TArray<const T*> GetAllFragmentOfType() const;
	
	void AssimilateInventoryFragments(UInv_CompositeBase* Composite) const;

private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	EInv_ItemCategory ItemCategory {EInv_ItemCategory::None};
	
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "GameItems"))
	FGameplayTag ItemType;
	
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FInv_ItemFragmentBase>> ItemFragments;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<AActor> PickUpActorClass;
};

template<typename T> requires std::derived_from<T, FInv_ItemFragmentBase>
	const T* FInv_ItemManifest::GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const
{
	for (const auto& Fragment : ItemFragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			if (!FragmentPtr->GetFragmentTag().MatchesTagExact(FragmentTag)) continue;
			return FragmentPtr;
		}
	}
	return nullptr;
}

template <typename T> requires std::derived_from<T, FInv_ItemFragmentBase>
const T* FInv_ItemManifest::GetFragmentOfType() const
{
	for (const TInstancedStruct<FInv_ItemFragmentBase>& Fragment : ItemFragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}

template<typename T> requires std::derived_from<T, FInv_ItemFragmentBase>
T* FInv_ItemManifest::GetFragmentOfTypeMutable()
{
	for (TInstancedStruct<FInv_ItemFragmentBase>& Fragment : ItemFragments)
	{
		if (T* FragmentPtr = Fragment.GetMutablePtr<T>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}

template <typename T> requires std::derived_from<T, FInv_ItemFragmentBase>
TArray<const T*> FInv_ItemManifest::GetAllFragmentOfType() const
{
	TArray<const T*> Results;
	for (const auto& Fragment : ItemFragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			Results.Add(FragmentPtr);
		}
	}
	return Results;
}
