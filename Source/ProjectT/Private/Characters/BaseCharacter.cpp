// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputTriggers.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/UserWidget.h"

#include "Components/InventoryComponent.h"
#include "Items/BaseItem.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	InventoryComponent->SetIsReplicated(true);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerController = Cast<APlayerController>(GetController());
	if (PlayerController)
	{
		PlayerController->SetShowMouseCursor(true);
	}

	if (PickupWidgetClass)
	{
		PickupWidget = CreateWidget(GetWorld(), PickupWidgetClass);
	}
}

void ABaseCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			Subsystem->ClearAllMappings();
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/** Move */
	if (bIsMovePressed)
	{
		FHitResult HitResult;
		PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult);
		if (HitResult.GetActor())
		{

			if (bIsFirst)
			{
				bIsFirst = false;
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, GetActorLocation());
			}

			FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitResult.Location);

			SetActorRotation(FMath::RInterpTo(GetActorRotation(), FRotator(GetActorRotation().Pitch, Rotation.Yaw, GetActorRotation().Roll), DeltaTime, 15.f));
			AddMovementInput(GetActorForwardVector());
		}
	}
}

void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Started, this, &ABaseCharacter::OnMoveAction);
			PlayerEnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Completed, this, &ABaseCharacter::OnMoveActionEnd);
		}
		if (JumpInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &ABaseCharacter::OnJumpAction);
			PlayerEnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Completed, this, &ABaseCharacter::OnJumpActionEnd);
		}
		if (InventoryAction)
		{
			PlayerEnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &ABaseCharacter::OnInventoryPressed);
		}
		if (Interaction)
		{
			PlayerEnhancedInputComponent->BindAction(Interaction, ETriggerEvent::Started, this, &ABaseCharacter::OnInteractionPressed);
		}
	}
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (InventoryComponent)
	{
		InventoryComponent->Character = this;
	}
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseCharacter, OverlappingItems, COND_OwnerOnly);
}

void ABaseCharacter::OnMoveAction()
{
	bIsMovePressed = true;
}

void ABaseCharacter::OnMoveActionEnd()
{
	bIsMovePressed = false;

	FHitResult HitResult;
	PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult);
	if (HitResult.GetActor())
	{
		bIsFirst = true;
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, HitResult.Location);
	}
}

void ABaseCharacter::OnJumpAction()
{
}

void ABaseCharacter::OnJumpActionEnd()
{
}

void ABaseCharacter::Landed(const FHitResult& Hit)
{
}

void ABaseCharacter::OnInventoryPressed()
{
	InventoryComponent->ToggleInventory();
}

void ABaseCharacter::OnInteractionPressed()
{
	if (OverlappingItems.Num() != 0)
	{
		ABaseItem* Item = OverlappingItems.Pop();
		InventoryComponent->AddItem(Item);
		if (OverlappingItems.Num() == 0)
		{
			PickupWidget->RemoveFromParent();
		}
		ServerDestryoItem(Item);
		return;
	}
}

void ABaseCharacter::ServerDestryoItem_Implementation(ABaseItem* Item)
{
	Item->Destroy();
}

void ABaseCharacter::OnRep_OverlappingItems()
{
	if (OverlappingItems.Num() != 0 && !PickupWidget->IsInViewport())
	{
		PickupWidget->AddToViewport();
	}
	else
	{
		PickupWidget->RemoveFromParent();
	}
}