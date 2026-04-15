// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_ItemPopUp.generated.h"

class USizeBox;
class UTextBlock;
class USlider;
class UButton;

DECLARE_DYNAMIC_DELEGATE_TwoParams(FPopMenuSplit, int32, SplitAmount, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopMenuDrop, int32, Index);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPopUpMenuConsume, int32, Index);

/**
 * The item popup widget shows up when right-clicking on an item
 * in the inventory grid.
 */
UCLASS()
class INVENTORYPLUGIN_API UInv_ItemPopUp : public UUserWidget
{
	GENERATED_BODY()
		
public:
	
	FPopMenuSplit OnSplit;
	FPopMenuDrop OnDrop;
	FPopUpMenuConsume OnConsume;
	
	int32 GetSplitAmount() const;
	void CollapseSplitButton() const;
	void CollapseConsumeButton() const;
	void SetSliderParams(const float Max, const float CurrentValue) const;
	FVector2D GetBoxSize() const;
	void SetGridIndex(const int32 Index) { this->GridIndex = Index;}
	int32 GetGridIndex() const {return this->GridIndex;}
	
protected:
	
	virtual void NativeOnInitialized() override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

private:
	
	UFUNCTION()
	void OnSplitButtonClicked();
	
	UFUNCTION()
	void OnDropButtonClicked();
	
	UFUNCTION()
	void OnConsumeButtonClicked();
	
	UFUNCTION()
	void OnSliderValueChanged(float Value);
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Split;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Drop;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Consume;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_Split;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SplitAmount;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;
	
	int32 GridIndex{INDEX_NONE}; /*To allow the pop-up menu to know which inventory item it belongs to*/

};
