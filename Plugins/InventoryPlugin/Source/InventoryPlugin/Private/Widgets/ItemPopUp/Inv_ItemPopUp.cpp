// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ItemPopUp/Inv_ItemPopUp.h"
#include "Components/Button.h"
#include "Components/Slider.h"

void UInv_ItemPopUp::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Button_Split->OnClicked.AddDynamic(this, &ThisClass::UInv_ItemPopUp::OnSplitButtonClicked);
	Button_Consume->OnClicked.AddDynamic(this, &ThisClass::UInv_ItemPopUp::OnConsumeButtonClicked);
	Button_Drop->OnClicked.AddDynamic(this, &ThisClass::UInv_ItemPopUp::OnDropButtonClicked);
	Slider_Split->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderValueChanged);
	
}

void UInv_ItemPopUp::OnSplitButtonClicked()
{
}

void UInv_ItemPopUp::OnDropButtonClicked()
{
}

void UInv_ItemPopUp::OnConsumeButtonClicked()
{
}

void UInv_ItemPopUp::OnSliderValueChanged(float Value)
{
}
