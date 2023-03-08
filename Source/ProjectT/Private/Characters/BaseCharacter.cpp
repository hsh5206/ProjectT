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
#include "Components/CombatComponent.h"
#include "Items/BaseItem.h"
#include "AbilitySystem/PTAbilitySystemComponent.h"
#include "AbilitySystem/PTAttributeSet.h"
#include "AbilitySystem/PTGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"

ABaseCharacter::ABaseCharacter()
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

	AbilitySystemComponent = CreateDefaultSubobject<UPTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	Attributes = CreateDefaultSubobject<UPTAttributeSet>(TEXT("AttributeSet"));
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
		if (BasicAttackAction)
		{
			PlayerEnhancedInputComponent->BindAction(BasicAttackAction, ETriggerEvent::Triggered, this, &ABaseCharacter::BasicAttack);
		}
	}

	/** GAS Binding */
	/*if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds("Confirm", "Cancle", "EPTAbilityInputID", static_cast<int32>(EPTAbilityInputID::Confirm), static_cast<int32>(EPTAbilityInputID::Cancel));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}*/
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
	DOREPLIFETIME_CONDITION(ABaseCharacter, CanNextCombo, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(ABaseCharacter, CurrentCombo, COND_OwnerOnly);

}

void ABaseCharacter::OnMoveAction()
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

void ABaseCharacter::OnMoveActionEnd()
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

void ABaseCharacter::MulticastSetActorRotationToMousePointer_Implementation(FRotator Rotation)
{
	SetActorRotation(FRotator(GetActorRotation().Pitch, Rotation.Yaw, GetActorRotation().Roll));
}

void ABaseCharacter::ServerSetActorRotationToMousePointer_Implementation(FRotator Rotation)
{
	MulticastSetActorRotationToMousePointer(Rotation);
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

void ABaseCharacter::BasicAttack()
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

		FGameplayEventData Payload;

		Payload.Instigator = this;
		Payload.EventTag = BasicAttackEventTag;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, BasicAttackEventTag, Payload);
	}
}

void ABaseCharacter::AttackStartComboState()
{
	CanNextCombo = true;
}

void ABaseCharacter::AttackEndComboState()
{
	CanNextCombo = false;
	CurrentCombo = 0;
}

void ABaseCharacter::ServerPlayMontage_Implementation(UAnimMontage* Montage, FName SectionName)
{
	MulticastPlayMontage(Montage, SectionName);
}

void ABaseCharacter::MulticastPlayMontage_Implementation(UAnimMontage* Montage, FName SectionName)
{
	GetMesh()->GetAnimInstance()->Montage_Play(Montage);
	GetMesh()->GetAnimInstance()->Montage_JumpToSection(SectionName, Montage);
}

void ABaseCharacter::ServerStopMontage_Implementation(UAnimMontage* Montage)
{
	MulticastStopMontage(Montage);
}

void ABaseCharacter::MulticastStopMontage_Implementation(UAnimMontage* Montage)
{
	GetMesh()->GetAnimInstance()->Montage_Stop(0.2f, Montage);
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

UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


/**
*
*	GAS
*
*/
void ABaseCharacter::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void ABaseCharacter::GiveAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (TSubclassOf<UPTGameplayAbility>& StartupAbility : DefaultsAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartupAbility));
			/*AbilitySystemComponent->GiveAbility(
				FGameplayAbilitySpec(StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this)
			);*/
		}
	}
}

// 서버에서 초기화 (서버만 어빌리티를 가지고 있기)
void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// AbilitySystem의 owner와 avatar세팅
	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	InitializeAttributes();
	GiveAbilities();
}

// 클라이언트에서 초기화
void ABaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();

	// Input에 Ability Binding
	/*if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds("Confirm", "Cancle", "EPTAbilityInputID", static_cast<int32>(EPTAbilityInputID::Confirm), static_cast<int32>(EPTAbilityInputID::Cancel));
		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}*/
}
