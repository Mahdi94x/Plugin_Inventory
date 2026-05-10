// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inv_ProxyMesh.generated.h"

class UInv_EquipmentComponent;

UCLASS()
class INVENTORYPLUGIN_API AInv_ProxyMesh : public AActor
{
	GENERATED_BODY()

public:
	AInv_ProxyMesh();

protected:
	virtual void BeginPlay() override;
	
private:
	
	FTimerHandle TimerForNextTick;
	void DelayedInitializeOwner(); /*Callback for the timer*/
	void DelayedInitialization();
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UInv_EquipmentComponent> ProxyEquipmentComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> ProxyMesh;
	
	// The mesh on the player-controlled character
	TWeakObjectPtr<USkeletalMeshComponent> SourceMesh;
};
