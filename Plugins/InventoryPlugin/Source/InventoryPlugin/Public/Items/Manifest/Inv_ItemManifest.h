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
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	EInv_ItemCategory ItemCategory {EInv_ItemCategory::None};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag ItemType;
	
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FInv_ItemFragmentBase>> ItemFragments;
};

