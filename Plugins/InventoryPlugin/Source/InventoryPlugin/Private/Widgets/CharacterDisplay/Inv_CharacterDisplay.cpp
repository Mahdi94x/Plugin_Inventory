// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/CharacterDisplay/Inv_CharacterDisplay.h"

#include "Blueprint/WidgetLayoutLibrary.h"
#include "EquipmentManagement/ProxyMesh/Inv_ProxyMesh.h"
#include "Kismet/GameplayStatics.h"

FReply UInv_CharacterDisplay::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	/*Avoid Snapping*/
	CurrentFramePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	LastFramePosition = CurrentFramePosition;
	
	bIsDragging = true;
	return FReply::Handled();
}

FReply UInv_CharacterDisplay::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	bIsDragging = false;
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UInv_CharacterDisplay::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

void UInv_CharacterDisplay::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(this, AInv_ProxyMesh::StaticClass(),OutActors);
	if (!OutActors.IsValidIndex(0)) return;
	
	AInv_ProxyMesh* ProxyMesh = Cast<AInv_ProxyMesh>(OutActors[0]);
	if (!IsValid(ProxyMesh)) return;
	
	Mesh = ProxyMesh->GetProxyMesh();
	
}

void UInv_CharacterDisplay::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (!bIsDragging) return;
	
	LastFramePosition = CurrentFramePosition;
	CurrentFramePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(this);
	
	const float HorizontalDelta = LastFramePosition.X - CurrentFramePosition.X;
	
	if (!Mesh.IsValid()) return;
	Mesh->AddRelativeRotation(FRotator(0.f, HorizontalDelta, 0.f));
}
