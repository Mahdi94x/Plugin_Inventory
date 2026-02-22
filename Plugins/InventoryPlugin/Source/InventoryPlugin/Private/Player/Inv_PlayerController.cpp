// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/Inv_PlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"
#include "Interaction/Inv_HighlightableInterface.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "InventoryManagment/Components/Inv_InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/HUD/Inv_HUDWidget.h"

AInv_PlayerController::AInv_PlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	TraceLenght = 500.0;
	ItemCollisionChannel = ECC_GameTraceChannel1;
}

void AInv_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem))
	{
		for (UInputMappingContext* CurrentContext : DefaultIMCs)
		{
			Subsystem->AddMappingContext(CurrentContext, 0);
		}
	}
	
	InventoryComponent = FindComponentByClass<UInv_InventoryComponent>();
	
	CreateHUDWidget();
	
}

void AInv_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	if (IsValid(EnhancedInputComponent))
	{
		EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started,this, &AInv_PlayerController::PrimaryInteract);
		EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started,this, &AInv_PlayerController::ToggleInventory);
	}
}

void AInv_PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TraceForItem();
}

void AInv_PlayerController::ToggleInventory()
{
	if (!InventoryComponent.IsValid()) return;
	InventoryComponent->ToggleInventoryMenu();
}

void AInv_PlayerController::PrimaryInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Primary Interact"));
}

void AInv_PlayerController::CreateHUDWidget()
{
	if (!IsLocalController()) return;
	
	HUDWidget = CreateWidget<UInv_HUDWidget>(this, HUDWidgetClass);
	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
}

void AInv_PlayerController::TraceForItem()
{
	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return;
	
	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	const FVector2D ViewportCenter = ViewportSize / 2.0f;
	
	FVector TraceStart;
	FVector ForwardVector;
	if (!UGameplayStatics::DeprojectScreenToWorld(this, ViewportCenter,TraceStart, ForwardVector)) return;
	
	const FVector TraceEnd = TraceStart + (ForwardVector * TraceLenght);
	FHitResult Hit;
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ItemCollisionChannel);
	
	LastActor = CurrentActor;
	CurrentActor = Hit.GetActor();
	
	if (!CurrentActor.IsValid())
	{
		if (IsValid(HUDWidget)) HUDWidget->HidePickupMessage();
	}
	
	if (CurrentActor == LastActor) return;
	
	if (CurrentActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Starting Tracing a new actor"));
		
		if (UActorComponent* Highlightable = CurrentActor->FindComponentByInterface(UInv_HighlightableInterface::StaticClass()); IsValid(Highlightable))
		{
			IInv_HighlightableInterface::Execute_Highlight(Highlightable);
		}
		
		UInv_ItemComponent* ItemComponent =  CurrentActor->FindComponentByClass<UInv_ItemComponent>();
		if (!IsValid(ItemComponent)) return;
		if (IsValid(HUDWidget)) HUDWidget->ShowPickupMessage(ItemComponent->GetPickupMessage());
		
	}
	
	if (LastActor.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Stopped Tracing the last actor"));
		
		if (UActorComponent* Highlightable = LastActor->FindComponentByInterface(UInv_HighlightableInterface::StaticClass()); IsValid(Highlightable))
		{
			IInv_HighlightableInterface::Execute_Unhighlight(Highlightable);
		}
	}
	
	
}
