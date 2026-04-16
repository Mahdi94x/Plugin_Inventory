// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_GridSlot.generated.h"

class UInv_ItemPopUp;
class UImage;
class UInv_InventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridSlotEvent, int32, GridIndex, const FPointerEvent&, MouseEvent);

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
	
	void SetItemPopUp(UInv_ItemPopUp* PopUp);
	UInv_ItemPopUp* GetItemPopUp() const;
	
	FGridSlotEvent GridSlotClicked;
	FGridSlotEvent GridSlotHovered;
	FGridSlotEvent GridSlotUnhovered;
	
protected:
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	
	UFUNCTION()
	void OnItemPopUpDestruct(UUserWidget* Menu);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;
	
	int32 TileIndex{INDEX_NONE};
	int32 UpperLeftIndex{INDEX_NONE};
	int32 StackCount{0};
	bool bIsGridSlotAvailable{true};
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	TWeakObjectPtr<UInv_ItemPopUp> ItemPopUp;
	
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
