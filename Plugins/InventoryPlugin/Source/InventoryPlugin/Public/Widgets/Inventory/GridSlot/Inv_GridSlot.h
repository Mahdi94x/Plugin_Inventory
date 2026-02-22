// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_GridSlot.generated.h"

class UImage;

UCLASS()
class INVENTORYPLUGIN_API UInv_GridSlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTileIndex(const int32 Index) {this->TileIndex = Index; }
	int32 GetTileIndex() const {return this->TileIndex;}
	
private:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;
	
	int32 TileIndex;
};
