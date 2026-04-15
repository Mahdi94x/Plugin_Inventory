// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/ItemPopUp/Inv_ItemPopUp.h"
#include "Components/Button.h"
#include "Components/SizeBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

void UInv_ItemPopUp::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Button_Split->OnClicked.AddDynamic(this, &ThisClass::UInv_ItemPopUp::OnSplitButtonClicked);
	Button_Consume->OnClicked.AddDynamic(this, &ThisClass::UInv_ItemPopUp::OnConsumeButtonClicked);
	Button_Drop->OnClicked.AddDynamic(this, &ThisClass::UInv_ItemPopUp::OnDropButtonClicked);
	Slider_Split->OnValueChanged.AddDynamic(this, &ThisClass::OnSliderValueChanged);
	
}

void UInv_ItemPopUp::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	this->RemoveFromParent();
}

int32 UInv_ItemPopUp::GetSplitAmount() const
{
	return FMath::Floor(Slider_Split->GetValue());
}

void UInv_ItemPopUp::OnSplitButtonClicked()
{
	if (OnSplit.ExecuteIfBound(GetSplitAmount(), GridIndex))
	{
		this->RemoveFromParent();
	}
}

void UInv_ItemPopUp::OnDropButtonClicked()
{
	if (OnDrop.ExecuteIfBound(GridIndex))
	{
		this->RemoveFromParent();
	}
}

void UInv_ItemPopUp::OnConsumeButtonClicked()
{
	if (OnConsume.ExecuteIfBound(GridIndex))
	{
		this->RemoveFromParent();
	}
}

void UInv_ItemPopUp::OnSliderValueChanged(float Value)
{
	Text_SplitAmount->SetText(FText::AsNumber(FMath::Floor(Value)));
}

void UInv_ItemPopUp::CollapseSplitButton() const
{
	Button_Split->SetVisibility(ESlateVisibility::Collapsed);
	Slider_Split->SetVisibility(ESlateVisibility::Collapsed);
	Text_SplitAmount->SetVisibility(ESlateVisibility::Collapsed);
}

void UInv_ItemPopUp::CollapseConsumeButton() const
{
	Button_Consume->SetVisibility(ESlateVisibility::Collapsed);
}

void UInv_ItemPopUp::SetSliderParams(const float Max, const float CurrentValue) const
{
	Slider_Split->SetMaxValue(Max);
	Slider_Split->SetMinValue(1);
	Slider_Split->SetValue(CurrentValue);
	Text_SplitAmount->SetText(FText::AsNumber(FMath::Floor(CurrentValue)));
}

FVector2D UInv_ItemPopUp::GetBoxSize() const
{
	return FVector2D(SizeBox_Root->GetWidthOverride(), SizeBox_Root->GetHeightOverride());
}
