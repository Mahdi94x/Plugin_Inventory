// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Inv_HighlightableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UInv_HighlightableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class INVENTORYPLUGIN_API IInv_HighlightableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	UFUNCTION(BlueprintnativeEvent, Category = "Inventory")
	void Highlight();
	
	UFUNCTION(BlueprintnativeEvent, Category = "Inventory")
	void Unhighlight();
};
