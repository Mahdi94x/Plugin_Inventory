// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Inv_HighlightableInterface.h"
#include "Components/StaticMeshComponent.h"
#include "Inv_HighlightableStaticMesh.generated.h"

UCLASS()
class INVENTORYPLUGIN_API UInv_HighlightableStaticMesh : public UStaticMeshComponent, public IInv_HighlightableInterface
{
	GENERATED_BODY()
	
public:
	virtual void Highlight_Implementation() override;
	virtual void Unhighlight_Implementation() override;
	
private:
	UPROPERTY(EditAnywhere, Category="Inventory")
	TObjectPtr<UMaterialInterface> HighlightMaterial;
};
