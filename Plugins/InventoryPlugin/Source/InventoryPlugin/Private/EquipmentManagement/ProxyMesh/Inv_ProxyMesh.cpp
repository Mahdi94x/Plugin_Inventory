// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentManagement/ProxyMesh/Inv_ProxyMesh.h"
#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"
#include "GameFramework/Character.h"

AInv_ProxyMesh::AInv_ProxyMesh()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(false);
	
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	
	ProxyMesh = CreateDefaultSubobject<USkeletalMeshComponent>("ProxyMesh");
	ProxyMesh->SetupAttachment(RootComponent);
	
	ProxyEquipmentComponent = CreateDefaultSubobject<UInv_EquipmentComponent>("ProxyEquipmentComponent");
	ProxyEquipmentComponent->SetOwningSkeletalMesh(ProxyMesh);
	ProxyEquipmentComponent->SetIsProxy(true);
}

void AInv_ProxyMesh::BeginPlay()
{
	Super::BeginPlay();
	DelayedInitializeOwner();
}

void AInv_ProxyMesh::DelayedInitializeOwner()
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		DelayedInitialization();
		return;
	}
	
	APlayerController* PC = World->GetFirstPlayerController();
	if (!IsValid(PC))
	{
		DelayedInitialization();
		return;
	}
	
	ACharacter* Character = Cast<ACharacter>(PC->GetPawn());
	if (!IsValid(Character))
	{
		DelayedInitialization();
		return;
	}
	
	USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	if (!IsValid(CharacterMesh))
	{
		DelayedInitialization();
		return;
	}
	
	SourceMesh = CharacterMesh;
	
	ProxyMesh->SetSkeletalMesh(SourceMesh->GetSkeletalMeshAsset());
	ProxyMesh->SetAnimInstanceClass(SourceMesh->GetAnimInstance()->GetClass());
	
	ProxyEquipmentComponent->InitializeOwner(PC);
}

void AInv_ProxyMesh::DelayedInitialization()
{
	/*Recall bounded function next tick if required*/
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUObject(this, &ThisClass::DelayedInitializeOwner);
	GetWorld()->GetTimerManager().SetTimerForNextTick(TimerDelegate);
}
