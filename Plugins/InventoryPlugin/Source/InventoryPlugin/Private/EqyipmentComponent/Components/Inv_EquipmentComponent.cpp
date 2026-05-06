// Fill out your copyright notice in the Description page of Project Settings.

#include "EquipmentManagement/Components/Inv_EquipmentComponent.h"

#include "EquipmentManagement/EquipActor/Inv_EquipActor.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "InventoryManagment/Components/Inv_InventoryComponent.h"
#include "InventoryManagment/Utils/Inv_InventoryStatics.h"
#include "Items/Inv_InventoryItem.h"
#include "Items/Fragments/Inv_ItemFragment.h"

void UInv_EquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
	
	OwningPlayerController = Cast<APlayerController>(GetOwner());
	
	if (OwningPlayerController.IsValid())
	{
		if (const ACharacter* OwnerCharacter = Cast<ACharacter>(OwningPlayerController->GetPawn()); IsValid(OwnerCharacter))
		{
			OwningSkeletalMesh = OwnerCharacter->GetMesh();
		}
		InitInventoryComponent();
	}
}

void UInv_EquipmentComponent::InitInventoryComponent()
{
	InventoryComponent = UInv_InventoryStatics::GetInventoryComponent(OwningPlayerController.Get());
	if (!InventoryComponent.IsValid()) return;
	
	if (!InventoryComponent->OnItemEquipped.IsAlreadyBound(this, &ThisClass::OnItemEquipped))
	{
		InventoryComponent->OnItemEquipped.AddDynamic(this, &ThisClass::OnItemEquipped);
	}
	
	if (!InventoryComponent->OnItemUnequipped.IsAlreadyBound(this, &ThisClass::OnItemUnequipped))
	{
		InventoryComponent->OnItemUnequipped.AddDynamic(this, &ThisClass::OnItemUnequipped);
	}

}

void UInv_EquipmentComponent::OnItemEquipped(UInv_InventoryItem* EquippedItem)
{
	if (!IsValid(EquippedItem)) return;
	if (!OwningPlayerController->HasAuthority()) return;
	
	FInv_ItemManifest& ItemManifest = EquippedItem->GetItemManifestMutable();
	FInv_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FInv_EquipmentFragment>();
	if (!(EquipmentFragment)) return;
	
	EquipmentFragment->OnEquip_Composite(OwningPlayerController.Get());
	
	if (!OwningSkeletalMesh.IsValid()) return;
	AInv_EquipActor* SpawnedEquipActor =  SpawnEquippedActor(EquipmentFragment, ItemManifest, OwningSkeletalMesh.Get());
	
	EquippedActorsArray.Add(SpawnedEquipActor);
	
}

AInv_EquipActor* UInv_EquipmentComponent::SpawnEquippedActor(FInv_EquipmentFragment* EquipmentFragment,
	const FInv_ItemManifest& Manifest, USkeletalMeshComponent* AttachMesh)
{
	AInv_EquipActor* SpawnedEquippedActor = EquipmentFragment->SpawnAttachedActor(AttachMesh);
	
	SpawnedEquippedActor->SetEquipmentType(EquipmentFragment->GetEquipmentTag());
	SpawnedEquippedActor->SetOwner(this->GetOwner());
	EquipmentFragment->SetEquippedActor(SpawnedEquippedActor);
	
	return SpawnedEquippedActor;
}

void UInv_EquipmentComponent::OnItemUnequipped(UInv_InventoryItem* UnequippedItem)
{
	if (!IsValid(UnequippedItem)) return;
	if (!OwningPlayerController->HasAuthority()) return;
	
	FInv_ItemManifest& ItemManifest = UnequippedItem->GetItemManifestMutable();
	FInv_EquipmentFragment* EquipmentFragment = ItemManifest.GetFragmentOfTypeMutable<FInv_EquipmentFragment>();
	if (!(EquipmentFragment)) return;
	
	EquipmentFragment->OnUnequip_Composite(OwningPlayerController.Get());
}


