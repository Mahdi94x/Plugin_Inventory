// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_SlottedItem.generated.h"


class UTextBlock;
class UInv_InventoryItem;
class UImage;

UCLASS()
class INVENTORYPLUGIN_API UInv_SlottedItem : public UUserWidget
{
	GENERATED_BODY()
	
	
public:
	bool IsStackable() const {return this->bIsStackable;}
	void SetIsStackable(bool bStackable) {this->bIsStackable = bStackable;}
	
	UImage* GetImageIcon() const {return this->Image_Icon;}
	
	void SetGridIndex(const int32 Index) {this->GridIndex = Index;}
	int32 GetGridIndex() const {return this->GridIndex;}
	
	void SetGridDimensions(const FIntPoint& Dimensions) {this->GridDimensions = Dimensions;}
	FIntPoint GetGridDimensions() const {return this->GridDimensions;}
	
	void SetInventoryItem(UInv_InventoryItem* Item);
	UInv_InventoryItem* GetInventoryItem() const {return this->InventoryItem.Get();}
	
	void SetImageBrush(const FSlateBrush& Brush) const;
	
	void UpdateStackCount(int32 StackCount);

private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;
	
	int32 GridIndex;
	FIntPoint GridDimensions;
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	bool bIsStackable{false};
};
