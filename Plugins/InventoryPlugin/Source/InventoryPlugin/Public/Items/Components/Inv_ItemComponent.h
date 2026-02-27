// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Items/Manifest/Inv_ItemManifest.h"
#include "Inv_ItemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent),Blueprintable)
class INVENTORYPLUGIN_API UInv_ItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInv_ItemComponent();
	FString GetPickupMessage() const {return this->PickupMessage;}
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	FInv_ItemManifest GetItemComponent_Manifest() const {return this->ItemComponent_Manifest;}

protected:


private:
	UPROPERTY(EditAnywhere, Replicated, Category = "Inventory")
	FInv_ItemManifest ItemComponent_Manifest;
	
	UPROPERTY(editanywhere, Category = "Inventory")
	FString PickupMessage;
};
