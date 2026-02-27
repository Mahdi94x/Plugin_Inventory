// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Types/Inv_GridTypes.h"
#include "Inv_InventoryGrid.generated.h"

class UInv_InventoryItem;
class UInv_InventoryComponent;
class UCanvasPanel;
class UInv_GridSlot;

UCLASS()
class INVENTORYPLUGIN_API UInv_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	EInv_ItemCategory GetItemCategory() const {return this->ItemCategory;}
	
	UFUNCTION()
	void AddItem(UInv_InventoryItem* InventoryItem);
	
private:
	
	void ConstructGridSlots();
	
	bool MatchesCategory(const UInv_InventoryItem* InventoryItem) const;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"), Category = "Inventory")
	EInv_ItemCategory ItemCategory;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UInv_GridSlot> GridSlotClass;
	
	UPROPERTY()
	TArray<TObjectPtr<UInv_GridSlot>> GridSlotsArray;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Rows;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Columns;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float TileSize;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> SlotsCanvasPanel;
	
	TWeakObjectPtr<UInv_InventoryComponent> InventoryComponent;
};

