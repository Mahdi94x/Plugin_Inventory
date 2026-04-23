// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/Composite/Inv_Leaf_Icon.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"

void UInv_Leaf_Icon::SetImage(UTexture2D* Texture) const
{
	Image_Icon->SetBrushFromTexture(Texture);
}

void UInv_Leaf_Icon::SetBoxSize(const FVector2D& Size) const
{
	SizeBox_Icon->SetWidthOverride(Size.X);
	SizeBox_Icon->SetHeightOverride(Size.Y);
}

void UInv_Leaf_Icon::SetImageSize(const FVector2D& Size) const
{
	Image_Icon->SetDesiredSizeOverride(Size);
}

FVector2D UInv_Leaf_Icon::GetImageSize() const
{
	return this->Image_Icon->GetDesiredSize();
}
