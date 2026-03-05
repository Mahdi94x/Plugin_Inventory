#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Types/Inv_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "Inv_ItemManifest.generated.h"

/*
 * The Item Manifest contains the required data for creating a new Inv_InventoryItem
 */

struct FInv_ItemFragmentBase;
class UInv_InventoryItem;

USTRUCT(BlueprintType)
struct INVENTORYPLUGIN_API FInv_ItemManifest
{
	GENERATED_BODY()
	
	EInv_ItemCategory GetItemCategory() const {return this->ItemCategory; }
	UInv_InventoryItem* ManifestCreation(UObject* NewOuter);
	FGameplayTag GetItemType() const {return this->ItemType;}
	
	template<typename T> requires std::derived_from<T, FInv_ItemFragmentBase>
	const T* GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const;
	
	template<typename T> requires std::derived_from<T, FInv_ItemFragmentBase>
	const T* GetFragmentOfType() const;

private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	EInv_ItemCategory ItemCategory {EInv_ItemCategory::None};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag ItemType;
	
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FInv_ItemFragmentBase>> ItemFragments;
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
	for (const auto& Fragment : ItemFragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}

