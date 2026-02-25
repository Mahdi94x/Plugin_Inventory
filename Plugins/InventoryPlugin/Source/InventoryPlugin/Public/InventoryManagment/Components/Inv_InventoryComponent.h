// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inv_InventoryComponent.generated.h"

class UInv_ItemComponent;
class UInv_InventoryItem;
class UInv_InventoryBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemChange, UInv_InventoryItem*, Item);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNRoomInInventory);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable)
class INVENTORYPLUGIN_API UInv_InventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInv_InventoryComponent();
	
	void ToggleInventoryMenu();
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Inventoty")
	void TryAddItem(UInv_ItemComponent* Item);
	
	FInventoryItemChange OnItemAdded;
	FInventoryItemChange OnItemRemoved;
	FNRoomInInventory NoRoomInInventory;

protected:
	virtual void BeginPlay() override;

private:
	void ConstructInventory();
	
	UPROPERTY(EditAnywhere, Category="Inventory")
	TSubclassOf<UInv_InventoryBase> InventoryMenuClass;
	
	UPROPERTY()
	TObjectPtr<UInv_InventoryBase> InventoryMenu;
	
	TWeakObjectPtr<APlayerController> OwningController;
	
	bool bInventoryMenuOpen = false;
	void OpenInventoryMenu();
	void CloseInventoryMenu();

};
