#include "Items/Fragments/Inv_ItemFragment.h"
#include "Widgets/Composite/Inv_CompositeBase.h"

void FInv_HealthPotionFragment::OnConsume(APlayerController* PC)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f,FColor::Green,FString::Printf(TEXT("Health Potion Consumed! Healing by %f HP."),HealAmount));
	}
}

void FInv_ManaPotionFragment::OnConsume(APlayerController* PC)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f,FColor::Cyan,FString::Printf(TEXT("Mana Potion Consumed! Added %f MP."),ManaAmount));
	}
}

void FInv_InventoryItemFragment::Assimilate(UInv_CompositeBase* Composite) const
{
	if (!MatchesWidgetTag(Composite)) return;
	Composite->Expand();
}

bool FInv_InventoryItemFragment::MatchesWidgetTag(const UInv_CompositeBase* Composite) const
{
	return Composite->GetFragmentTag().MatchesTagExact(GetFragmentTag());
}
