// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Composite/Inv_Leaf_Text.h"
#include "Components/TextBlock.h"

void UInv_Leaf_Text::SetText(const FText& Text) const
{
	Text_Leaf_Text->SetText(Text);
}

void UInv_Leaf_Text::NativePreConstruct()
{
	Super::NativePreConstruct();
	
	FSlateFontInfo FontInfo = Text_Leaf_Text->GetFont();
	FontInfo.Size = FontSize;
	Text_Leaf_Text->SetFont(FontInfo);
}
