// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Composite/Inv_CompositeBase.h"

void UInv_CompositeBase::Collapse()
{
	this->SetVisibility(ESlateVisibility::Collapsed);
}

void UInv_CompositeBase::Expand()
{
	this->SetVisibility(ESlateVisibility::Visible);
}
