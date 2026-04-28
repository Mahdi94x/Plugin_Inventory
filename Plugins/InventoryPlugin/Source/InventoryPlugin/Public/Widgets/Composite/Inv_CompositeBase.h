#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "Inv_CompositeBase.generated.h"

UCLASS()
class INVENTORYPLUGIN_API UInv_CompositeBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void Collapse();
	void Expand();
	
	using FUncType = TFunction<void(UInv_CompositeBase*)>;
	virtual void ApplyFunction(FUncType Function){}

	FGameplayTag GetFragmentTag() const { return this->FragmentTag; }
	void SetFragmentTag(const FGameplayTag& Tag) { this->FragmentTag = Tag; }

private:
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "FragmentTags"))
	FGameplayTag FragmentTag;
};