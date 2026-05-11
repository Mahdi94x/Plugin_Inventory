// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inv_CharacterDisplay.generated.h"

UCLASS()
class INVENTORYPLUGIN_API UInv_CharacterDisplay : public UUserWidget
{
	GENERATED_BODY()
public:
	
protected:
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	
private:
	TWeakObjectPtr<USkeletalMeshComponent> Mesh;
	bool bIsDragging{false};
	FVector2D CurrentFramePosition;
	FVector2D LastFramePosition;
};
