// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Inv_InventoryItem.generated.h"

struct FInstancedStruct;
struct FInv_ItemManifest;

UCLASS()
class INVENTORYPLUGIN_API UInv_InventoryItem : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void SetItemManifest(const FInv_ItemManifest& Manifest);
	
private:
	UPROPERTY(VisibleAnywhere, Category = "Inventory", meta = (BaseStruct = "/Script/Plugin_Inventory.Inv_ItemManifest"), Replicated)
	FInstancedStruct ItemManifest;
};
