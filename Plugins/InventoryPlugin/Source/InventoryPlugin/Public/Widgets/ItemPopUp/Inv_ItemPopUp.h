// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_ItemPopUp.generated.h"

class USizeBox;
class UTextBlock;
class USlider;
class UButton;
/**
 * The item popup widget shows up when right-clicking on an item
 * in the inventory grid.
 */
UCLASS()
class INVENTORYPLUGIN_API UInv_ItemPopUp : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
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
	
	
	
	
};
