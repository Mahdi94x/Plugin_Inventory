#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Composite/Inv_CompositeBase.h"
#include "Widgets/Composite/Inv_Leaf_Icon.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"
#include "Widgets/Composite/Inv_Leaf_Text.h"


/*====================================================================================================================*/

/*====================================================================================================================*/

/*====================================================================================================================*/
void FInv_InventoryItemFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
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
	FInv_InventoryItemFragment::Assimilate(Composite);
	
	if (const UInv_Leaf_Text* TextLeaf = Cast<UInv_Leaf_Text>(Composite); IsValid(TextLeaf))
	{
		TextLeaf->SetText(FragmentText);
	}
}
/*====================================================================================================================*/
void FInv_LabeledNumberFragment::Assimilate(UInv_CompositeBase* Composite) const
{
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

