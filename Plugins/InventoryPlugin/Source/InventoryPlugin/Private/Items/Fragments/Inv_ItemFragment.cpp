#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Composite/Inv_CompositeBase.h"
#include "Widgets/Composite/Inv_Leaf_Icon.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"
#include "Widgets/Composite/Inv_Leaf_Text.h"

void FInv_HealthSubFragment::OnConsume_Leaf(APlayerController* PC)
{
	FInv_SubFragment::OnConsume_Leaf(PC);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,
			FString::Printf(TEXT("Health amount increased by: %.2f"),GetValue()));
	}
}
/*====================================================================================================================*/
void FInv_ManaSubFragment::OnConsume_Leaf(APlayerController* PC)
{
	FInv_SubFragment::OnConsume_Leaf(PC);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-2,5.f,FColor::Cyan,
			FString::Printf(TEXT("Mana amount increased by: %.2f"),GetValue()));
	}
}
/*====================================================================================================================*/
void FInv_ConsumptionFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	
	for (const auto& SubFragment : SubFragmentsArray)
	{
		if (const auto* SubRef = SubFragment.GetPtr<FInv_SubFragment>())
		{
			SubRef->Assimilate(Composite);
		}
	}
}
/*====================================================================================================================*/
void FInv_ConsumptionFragment::OnConsume_Composite(APlayerController* PC)
{
	for (auto& SubFragment : SubFragmentsArray)
	{
		if (auto* SubRef = SubFragment.GetMutablePtr<FInv_SubFragment>())
		{
			SubRef->OnConsume_Leaf(PC);
		}
	}
}
/*====================================================================================================================*/
void FInv_ConsumptionFragment::FragmentManifest()
{
	FInv_InventoryItemFragment::FragmentManifest();
	for (auto& SubFragment : SubFragmentsArray)
	{
		if (auto* SubRef = SubFragment.GetMutablePtr<FInv_SubFragment>())
		{
			SubRef->FragmentManifest();
		}
	}
}
/*====================================================================================================================*/
void FInv_InventoryItemFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	Composite->Expand();
}
/*====================================================================================================================*/
bool FInv_InventoryItemFragment::MatchesWidgetTag(const UInv_CompositeBase* Composite) const
{
	return Composite->GetFragmentTag().MatchesTagExact(GetFragmentTag());
}
/*====================================================================================================================*/
void FInv_IconFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	FInv_InventoryItemFragment::Assimilate(Composite);
	
	if (const UInv_Leaf_Icon* IconLeaf = Cast<UInv_Leaf_Icon>(Composite); IsValid(IconLeaf))
	{
		IconLeaf->SetImage(this->IconTexture);
		IconLeaf->SetBoxSize(IconDimension);
		IconLeaf->SetImageSize(IconDimension);
	}
}
/*====================================================================================================================*/
void FInv_TextFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	FInv_InventoryItemFragment::Assimilate(Composite);
	
	if (const UInv_Leaf_Text* TextLeaf = Cast<UInv_Leaf_Text>(Composite); IsValid(TextLeaf))
	{
		TextLeaf->SetText(FragmentText);
	}
}
/*====================================================================================================================*/
void FInv_LabeledNumberFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	FInv_InventoryItemFragment::Assimilate(Composite);
	
	if (const UInv_Leaf_LabeledValue* LabeledValueLeaf = Cast<UInv_Leaf_LabeledValue>(Composite); IsValid(LabeledValueLeaf))
	{
		LabeledValueLeaf->SetText_Label(Text_Label, bShouldCollapseLabel);
		
		FNumberFormattingOptions Options;
		Options.MinimumFractionalDigits = MinFractionalDigits;
		Options.MaximumFractionalDigits = MaxFractionalDigits;
		LabeledValueLeaf->SetText_Value(FText::AsNumber(Value, &Options), bShouldCollapseValue);
	}
}

void FInv_LabeledNumberFragment::FragmentManifest()
{
	FInv_InventoryItemFragment::FragmentManifest();
	
	if (bRandomizeOnManifest)
	{
		Value = FMath::FRandRange(MinValue, MaxValue);
	}
	bRandomizeOnManifest = false;
}
/*====================================================================================================================*/

