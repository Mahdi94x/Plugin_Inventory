// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_GridSlot.generated.h"

class UImage;
class UInv_InventoryItem;

UENUM(BlueprintType)
enum class EInv_GridSlotState : uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GrayedOut
};

UCLASS()
class INVENTORYPLUGIN_API UInv_GridSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTileIndex(const int32 Index) {this->TileIndex = Index; }
	int32 GetTileIndex() const {return this->TileIndex;}
	
	EInv_GridSlotState GetGridSlotState() const {return this->GridSlotState;}
	
	void SetOccupiedTexture();
	void SetUnoccupiedTexture();
	void SetSelectedTexture();
	void SetGrayedOutTexture();
	
	TWeakObjectPtr<UInv_InventoryItem> GetInventoryItem() const {return this->InventoryItem;}
	void SetInventoryItem(UInv_InventoryItem* Item);
	
	int32 GetUpperLeftIndex() const {return this->UpperLeftIndex;}
	void SetUpperLeftIndex(const int32 Index) {this->UpperLeftIndex = Index;}
	
	int32 GetStackCount() const {return this->StackCount;}
	void SetStackCount(const int32 Count) {this->StackCount = Count;}
	
	bool GetGridSlotAvailable() const {return this->bIsGridSlotAvailable;}
	void SetGridSlotAvailable(const bool Availability) {this->bIsGridSlotAvailable = Availability;} 
	
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;
	
	int32 TileIndex{INDEX_NONE};
	int32 StackCount{0};
	int32 UpperLeftIndex{INDEX_NONE};
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	bool bIsGridSlotAvailable{true};
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Unoccupied;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Occupied;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Selected;
	
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_GrayedOut;
	
	EInv_GridSlotState GridSlotState;

};
