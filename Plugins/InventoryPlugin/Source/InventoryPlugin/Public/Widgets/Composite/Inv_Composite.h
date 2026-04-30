// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inv_CompositeBase.h"
#include "Inv_Composite.generated.h"

/**
 * 
 */
UCLASS()
class INVENTORYPLUGIN_API UInv_Composite : public UInv_CompositeBase
{
	GENERATED_BODY()
	
public:
	virtual void ApplyFunction(FUncType Function) override;
	virtual void Collapse() override;
	TArray<UInv_CompositeBase*> GetChildren() { return Children; }
	
protected:
	virtual void NativeOnInitialized() override;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<UInv_CompositeBase>> Children;
};
