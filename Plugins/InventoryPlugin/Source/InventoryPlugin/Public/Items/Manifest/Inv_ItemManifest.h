#pragma once

#include "CoreMinimal.h"
#include "Types/Inv_GridTypes.h"
#include "StructUtils/InstancedStruct.h"
#include "Inv_ItemManifest.generated.h"

/*
 * The Item Manifest contains the required data for creating a new Inv_InventoryItem
 */

class UInv_InventoryItem;

USTRUCT(BlueprintType)
struct INVENTORYPLUGIN_API FInv_ItemManifest
{
	GENERATED_BODY()
	
	EInv_ItemCategory GetItemCategory() const {return this->ItemCategory; }
	UInv_InventoryItem* ManifestCreation(UObject* NewOuter);
	
private:
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	EInv_ItemCategory ItemCategory {EInv_ItemCategory::None};
};

