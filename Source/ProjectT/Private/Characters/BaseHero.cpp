// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseHero.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "InputTriggers.h"

#include "Components/InventoryComponent.h"
#include "Components/CombatComponent.h"
#include "Items/BaseItem.h"

#include "AbilitySystem/PTAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

ABaseHero::ABaseHero()
{
	PrimaryActorTick.bCanEverTick = true;
	SetReplicates(true);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Rune1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rune1"));
	Rune1->SetupAttachment(GetMesh(), FName("Rune1"));
	Rune1->SetIsReplicated(true);
	Rune2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rune2"));
	Rune2->SetupAttachment(GetMesh(), FName("Rune2"));
	Rune2->SetIsReplicated(true);
	Rune3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Rune3"));
	Rune3->SetupAttachment(GetMesh(), FName("Rune3"));
	Rune3->SetIsReplicated(true);

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("Inventory"));
	InventoryComponent->SetIsReplicated(true);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat"));
	CombatComponent->SetIsReplicated(true);
}

void ABaseHero::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/** Move */
	if (bIsMovePressed && !AbilitySystemComponent->HasAnyMatchingGameplayTags(CantMoveTag))
	{
		FHitResult HitResult;
		PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult);
		if (HitResult.GetActor())
		{

			//if (bIsFirst)
			//{
			//	bIsFirst = false;
			//	UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, GetActorLocation());
			//}

			FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitResult.Location);

			SetActorRotation(FMath::RInterpTo(GetActorRotation(), FRotator(GetActorRotation().Pitch, Rotation.Yaw, GetActorRotation().Roll), DeltaTime, 15.f));
			AddMovementInput(GetActorForwardVector());
		}
	}
}

void ABaseHero::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* PlayerEnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Started, this, &ABaseHero::OnMoveAction);
			PlayerEnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Completed, this, &ABaseHero::OnMoveActionEnd);
		}
		if (JumpInputAction)
		{
			PlayerEnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Started, this, &ABaseHero::OnJumpAction);
			PlayerEnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Completed, this, &ABaseHero::OnJumpActionEnd);
		}
		if (InventoryAction)
		{
			PlayerEnhancedInputComponent->BindAction(InventoryAction, ETriggerEvent::Started, this, &ABaseHero::OnInventoryPressed);
		}
		if (Interaction)
		{
			PlayerEnhancedInputComponent->BindAction(Interaction, ETriggerEvent::Started, this, &ABaseHero::OnInteractionPressed);
		}
		if (BasicAttackAction)
		{
			PlayerEnhancedInputComponent->BindAction(BasicAttackAction, ETriggerEvent::Triggered, this, &ABaseHero::BasicAttack);
		}
	}
}

void ABaseHero::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ABaseHero, OverlappingItems, COND_OwnerOnly);
}

void ABaseHero::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (InventoryComponent)
	{
		InventoryComponent->Character = this;
	}
}


void ABaseHero::BeginPlay()
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

void ABaseHero::PawnClientRestart()
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

void ABaseHero::OnMoveAction()
{
	bIsMovePressed = true;

	FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
	if (!MoveEffect.Get()) return;

	if (AbilitySystemComponent)
	{
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(MoveEffect, 1, EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			if (ActiveGEHandle.WasSuccessfullyApplied())
			{
				MoveEffectHandle = ActiveGEHandle;
			}
		}
	}
}

void ABaseHero::OnMoveActionEnd()
{
	bIsMovePressed = false;

	/*FHitResult HitResult;
	PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult);
	if (HitResult.GetActor())
	{
		bIsFirst = true;
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(PlayerController, HitResult.Location);
	}*/


	AbilitySystemComponent->RemoveActiveGameplayEffect(MoveEffectHandle);
}

void ABaseHero::OnJumpAction()
{
}

void ABaseHero::OnJumpActionEnd()
{
}

void ABaseHero::Landed(const FHitResult& Hit)
{
}

void ABaseHero::OnInventoryPressed()
{
	InventoryComponent->ToggleInventory();
}

void ABaseHero::MulticastSetActorRotationToMousePointer_Implementation(FRotator Rotation)
{
	SetActorRotation(FRotator(GetActorRotation().Pitch, Rotation.Yaw, GetActorRotation().Roll));
}

void ABaseHero::ServerSetActorRotationToMousePointer_Implementation(FRotator Rotation)
{
	MulticastSetActorRotationToMousePointer(Rotation);
}

void ABaseHero::OnInteractionPressed()
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

void ABaseHero::BasicAttack()
{
	if (CanNextCombo)
	{
		/*if (HasAuthority())
		{
			if (FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1))
			{
				CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
			}
			CanNextCombo = false;
		}
		else
		{
			ServerComboCombatStateChanged();
		}*/

		if (FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1))
		{
			CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
		}
		CanNextCombo = false;

		if (!HasAuthority())
		{
			ServerComboCombatStateChanged();
		}

		FGameplayEventData Payload;

		Payload.Instigator = this;
		Payload.EventTag = BasicAttackEventTag;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, BasicAttackEventTag, Payload);
	}
}

void ABaseHero::AttackStartComboState()
{
	CanNextCombo = true;
}

void ABaseHero::AttackEndComboState()
{
	CanNextCombo = false;
	CurrentCombo = 0;
}

void ABaseHero::ServerComboCombatStateChanged_Implementation()
{
	if (FMath::IsWithinInclusive<int32>(CurrentCombo, 0, MaxCombo - 1))
	{
		CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
	}
	CanNextCombo = false;
}

void ABaseHero::ServerAttackStartComboState_Implementation()
{
	CanNextCombo = true;
}

void ABaseHero::ServerAttackEndComboState_Implementation()
{
	CanNextCombo = false;
	CurrentCombo = 0;
}

void ABaseHero::OnRep_OverlappingItems()
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

void ABaseHero::ServerDestryoItem_Implementation(ABaseItem* Item)
{
	Item->Destroy();
}
