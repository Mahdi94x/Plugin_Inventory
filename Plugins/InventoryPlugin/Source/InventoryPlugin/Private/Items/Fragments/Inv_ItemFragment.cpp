#include "Items/Fragments/Inv_ItemFragment.h"

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