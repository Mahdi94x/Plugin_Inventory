// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Inv_HoverItem.generated.h"

class UTextBlock;
class UImage;
class UInv_InventoryItem;

UCLASS()
class INVENTORYPLUGIN_API UInv_HoverItem : public UUserWidget
{
	GENERATED_BODY()
	
public:
	
	void SetImageBruh(const FSlateBrush& Brush) const;
	void UpdateStackCount(const int32 Count) const;
	
	FGameplayTag GetItemType() const;
	int32 GetStackCount() const {return this->StackCount; }
	
	bool IsStackable() const {return this->bIsStackable;}
	void SetIsStackable(const bool bStacks);
	
	int32 GetPreviousGridIndex() const {return this->PreviousGridIndex;}
	void SetPreviousGridIndex(const int32 Index) {this->PreviousGridIndex = Index;}
	
	FIntPoint GetGridDimensions() const {return this->GridDimensions;}
	void SetGridDimensions(const FIntPoint& Dimensions) {this->GridDimensions = Dimensions;}
	
	UInv_InventoryItem* GetInventoryItem() const;
	void SetInventoryItem(UInv_InventoryItem* Item);

private:
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;
	
	int32 PreviousGridIndex;
	FIntPoint GridDimensions;
	TWeakObjectPtr<UInv_InventoryItem> InventoryItem;
	bool bIsStackable{false};
	int32 StackCount{0};
};
