// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inv_SlottedItem.h"
#include "GameplayTagContainer.h"
#include "Inv_EquippedSlottedItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEquippedSlottedItemClicked, class UInv_EquippedSlottedItem *, EquippedSlottedItem);

UCLASS()
class INVENTORYPLUGIN_API UInv_EquippedSlottedItem : public UInv_SlottedItem
{
	GENERATED_BODY()
	
public:
	void SetEquipmentTypeTag(const FGameplayTag& Tag) {this->EquipmentTypeTag = Tag;}
	FGameplayTag GetEquipmentTypeTag() const {return this->EquipmentTypeTag;}
	
	FEquippedSlottedItemClicked OnEquippedSlottedItemClicked;
	
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	
private:
	UPROPERTY()
	FGameplayTag EquipmentTypeTag;
};
