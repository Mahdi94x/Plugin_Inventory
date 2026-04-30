#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Composite/Inv_CompositeBase.h"
#include "Widgets/Composite/Inv_Leaf_Icon.h"
#include "Widgets/Composite/Inv_Leaf_LabeledValue.h"
#include "Widgets/Composite/Inv_Leaf_Text.h"

/*General Fragments*/
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
/*====================================================================================================================*/
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

/*Consumption Fragment*/
/*====================================================================================================================*/
void FInv_HealthConsumeModifier::OnConsume_Leaf(APlayerController* PC)
{
	FInv_ConsumeModifier::OnConsume_Leaf(PC);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,
			FString::Printf(TEXT("Health amount increased by: %.2f"),GetValue()));
	}
}
/*====================================================================================================================*/
void FInv_ManaConsumeModifier::OnConsume_Leaf(APlayerController* PC)
{
	FInv_ConsumeModifier::OnConsume_Leaf(PC);
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
	
	for (const auto& SubFragment : ConsumeModifiersArray)
	{
		if (const auto* SubRef = SubFragment.GetPtr<FInv_ConsumeModifier>())
		{
			SubRef->Assimilate(Composite);
		}
	}
}
/*====================================================================================================================*/
void FInv_ConsumptionFragment::OnConsume_Composite(APlayerController* PC)
{
	for (auto& SubFragment : ConsumeModifiersArray)
	{
		if (auto* SubRef = SubFragment.GetMutablePtr<FInv_ConsumeModifier>())
		{
			SubRef->OnConsume_Leaf(PC);
		}
	}
}
/*====================================================================================================================*/
void FInv_ConsumptionFragment::FragmentManifest()
{
	FInv_InventoryItemFragment::FragmentManifest();
	for (auto& SubFragment : ConsumeModifiersArray)
	{
		if (auto* SubRef = SubFragment.GetMutablePtr<FInv_ConsumeModifier>())
		{
			SubRef->FragmentManifest();
		}
	}
}
/*====================================================================================================================*/


/*Equipment Fragment*/
/*====================================================================================================================*/
void FInv_StrengthEquipModifier::OnEquip_Leaf(APlayerController* PC)
{
	FInv_EquipModifier::OnEquip_Leaf(PC);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Green,
			FString::Printf(TEXT("Strength increased by: %.2f"),GetValue()));
	}
}
/*====================================================================================================================*/

void FInv_StrengthEquipModifier::OnUnequip_Leaf(APlayerController* PC)
{
	FInv_EquipModifier::OnUnequip_Leaf(PC);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red,
			FString::Printf(TEXT("Strength decreased by: %.2f"),GetValue()));
	}
}
/*====================================================================================================================*/

void FInv_EquipmentFragment::OnEquip_Composite(APlayerController* PC)
{
	if (bEquipped) return;
	bEquipped = true;
	for (auto& SubFragment : EquipModifiersArray)
	{
		if (auto* SubRef = SubFragment.GetMutablePtr<FInv_EquipModifier>())
		{
			SubRef->OnEquip_Leaf(PC);
		}
	}
}
/*====================================================================================================================*/

void FInv_EquipmentFragment::OnUnequip_Composite(APlayerController* PC)
{
	if (!bEquipped) return;
	bEquipped = false;
	for (auto& SubFragment : EquipModifiersArray)
	{
		if (auto* SubRef = SubFragment.GetMutablePtr<FInv_EquipModifier>())
		{
			SubRef->OnUnequip_Leaf(PC);
		}
	}
}
/*====================================================================================================================*/
void FInv_EquipmentFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	FInv_InventoryItemFragment::Assimilate(Composite);
	for (const auto& SubFragment : EquipModifiersArray)
	{
		if (const auto* SubRef = SubFragment.GetPtr<FInv_EquipModifier>())
		{
			SubRef->Assimilate(Composite);
		}
	}
}
/*====================================================================================================================*/
void FInv_EquipmentFragment::FragmentManifest()
{
	FInv_InventoryItemFragment::FragmentManifest();
	for (auto& SubFragment : EquipModifiersArray)
	{
		if (auto* SubRef = SubFragment.GetMutablePtr<FInv_EquipModifier>())
		{
			SubRef->FragmentManifest();
		}
	}
}
/*====================================================================================================================*/
