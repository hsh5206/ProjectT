// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseHero.h"
#include "GameFrameWork/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "InputTriggers.h"
#include "Components/CapsuleComponent.h"

#include "Components/InventoryComponent.h"
#include "Components/CombatComponent.h"
#include "Items/BaseItem.h"
#include "Characters/Skills/Skill3_SpawnActor.h"
#include "Characters/Skills/SKill4DecalActor.h"
#include "Characters/BaseEnemy.h"
#include "Widgets/MainScreenWidget.h"
#include "AbilitySystem/PTAttributeSet.h"

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
		if (Skill_1_Action)
		{
			PlayerEnhancedInputComponent->BindAction(Skill_1_Action, ETriggerEvent::Started, this, &ABaseHero::Skill_1);
		}
		if (Skill_2_Action)
		{
			PlayerEnhancedInputComponent->BindAction(Skill_2_Action, ETriggerEvent::Started, this, &ABaseHero::Skill_2);
		}
		if (Skill_3_Action)
		{
			PlayerEnhancedInputComponent->BindAction(Skill_3_Action, ETriggerEvent::Started, this, &ABaseHero::Skill_3);
		}
		if (Skill_4_Action)
		{
			PlayerEnhancedInputComponent->BindAction(Skill_4_Action, ETriggerEvent::Started, this, &ABaseHero::Skill_4);
		}
		if (UseHP)
		{
			PlayerEnhancedInputComponent->BindAction(UseHP, ETriggerEvent::Started, this, &ABaseHero::UseHPPortion);
		}
		if (UseMP)
		{
			PlayerEnhancedInputComponent->BindAction(UseMP, ETriggerEvent::Started, this, &ABaseHero::UseMPPortion);
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
	if (MainWidgetClass)
	{
		MainWidget = CreateWidget<UMainScreenWidget>(GetWorld(), MainWidgetClass);
		MainWidget->AddToViewport();
		MainWidget->SetHPBarPercent(Attributes->GetHealth(), Attributes->GetMaxHealth());
		MainWidget->SetMPBarPercent(Attributes->GetMana(), Attributes->GetMaxMana());
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
	if (!InventoryComponent->EquippingWeapon) return;
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

void ABaseHero::Skill_1()
{
	UE_LOG(LogTemp, Warning, TEXT("SKill 1"));

	FGameplayEventData Payload;

	Payload.Instigator = this;

	FGameplayTag Skill_1_Tag = FGameplayTag::RequestGameplayTag(FName("Event.Attack.Skill1"));
	Payload.EventTag = Skill_1_Tag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, Skill_1_Tag, Payload);
}

void ABaseHero::Skill_1_DamageEvent()
{
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() + GetActorForwardVector() * 300.f;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	TArray<FHitResult> Hits;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);

	UKismetSystemLibrary::SphereTraceMultiForObjects(
		this,
		Start,
		End,
		100.f,
		ObjectTypes,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		Hits,
		true
	);

	if (Hits.Num() != 0)
	{
		for (FHitResult& Hit : Hits)
		{
			if (!IgnoreActors.Contains(Hit.GetActor()))
			{
				if (ABaseCharacter* Target = Cast<ABaseCharacter>(Hit.GetActor()))
				{
					if (Target->bIsDead) continue;
					FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
					EffectContext.AddHitResult(Hit);
					FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(SKill1Effect, 1, EffectContext);
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(FName("SkillDamageMultiplier"), 1.3f);
					if (SpecHandle.IsValid())
					{
						UPTAbilitySystemComponent* TargetASC = Target->AbilitySystemComponent;
						FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
					}
					IgnoreActors.AddUnique(Hit.GetActor());
				}
			}
		}	
	}
}

void ABaseHero::Skill_2()
{
	UE_LOG(LogTemp, Warning, TEXT("SKill 2"));

	FGameplayEventData Payload;

	Payload.Instigator = this;

	FGameplayTag Skill_2_Tag = FGameplayTag::RequestGameplayTag(FName("Event.Attack.Skill2"));
	Payload.EventTag = Skill_2_Tag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, Skill_2_Tag, Payload);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
}

void ABaseHero::Skill_2_DamageEvent()
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() + GetActorForwardVector() * -500.f;

	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	TArray<FHitResult> Hits; 
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	TEnumAsByte<EObjectTypeQuery> Pawn = UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn);
	ObjectTypes.Add(Pawn);

	UKismetSystemLibrary::SphereTraceMultiForObjects(
		this,
		Start,
		End,
		150.f,
		ObjectTypes,
		false,
		IgnoreActors,
		EDrawDebugTrace::None,
		Hits,
		true
	);

	if (Hits.Num() != 0)
	{
		for (FHitResult& Hit : Hits)
		{
			if (!IgnoreActors.Contains(Hit.GetActor()))
			{
				if (ABaseCharacter* Target = Cast<ABaseCharacter>(Hit.GetActor()))
				{
					if (Target->bIsDead) continue;

					FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
					EffectContext.AddHitResult(Hit);
					FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(SKill1Effect, 1, EffectContext);
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(FName("SkillDamageMultiplier"), 2.0f);
					if (SpecHandle.IsValid())
					{
						UPTAbilitySystemComponent* TargetASC = Target->AbilitySystemComponent;
						FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
					}
					IgnoreActors.AddUnique(Hit.GetActor());
				}
			}
		}
	}
}

void ABaseHero::Skill_3()
{
	UE_LOG(LogTemp, Warning, TEXT("SKill 3"));

	FGameplayEventData Payload;

	Payload.Instigator = this;

	FGameplayTag Skill_3_Tag = FGameplayTag::RequestGameplayTag(FName("Event.Attack.Skill3"));
	Payload.EventTag = Skill_3_Tag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, Skill_3_Tag, Payload);
}

void ABaseHero::Skill_3_DamageEvent()
{
	FVector Location = GetActorLocation() + GetActorForwardVector() * 30.f;
	FRotator Rotation(0);
	for (int i = 0; i < 3; i++)
	{
		ASkill3_SpawnActor* SpawnedActor = Cast<ASkill3_SpawnActor>(GetWorld()->SpawnActor(Skill3Actor, &Location, &Rotation));
		SpawnedActor->SetOwner(this);

		if (i == 0)
		{
			SpawnedActor->SetActorRotation(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw - 30.f, GetActorRotation().Roll));
		}
		else if (i == 1)
		{
			SpawnedActor->SetActorRotation(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw, GetActorRotation().Roll));
		}
		else
		{
			SpawnedActor->SetActorRotation(FRotator(GetActorRotation().Pitch, GetActorRotation().Yaw + 30.f, GetActorRotation().Roll));
		}
	}
}

void ABaseHero::Skill_4()
{
	if (Attributes->GetMana() >= 30.f)
	{
		UE_LOG(LogTemp, Warning, TEXT("SKill 4"));

		FGameplayEventData Payload;
		Payload.Instigator = this;
		FGameplayTag Skill_4_Tag = FGameplayTag::RequestGameplayTag(FName("Event.Attack.SKill4"));
		Payload.EventTag = Skill_4_Tag;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, Skill_4_Tag, Payload);

		FVector Location = GetActorLocation() + GetActorForwardVector() * 300.f;
		FRotator Rotation(0);
		SKill4DecalActor = Cast<ASKill4DecalActor>(GetWorld()->SpawnActor(Skill4Decal, &Location, &Rotation));
		SKill4DecalActor->SetOwner(this);
	}
}

void ABaseHero::Skill_4_DamageEvent()
{
	SKill4DecalActor->DamageToEnemies();
}

void ABaseHero::Skill_4_FinalDamageEvent()
{
	SKill4DecalActor->FinalDamageToEnemies();
	SKill4DecalActor->SetLifeSpan(0.1f);
	SKill4DecalActor = nullptr;
}

void ABaseHero::UseHPPortion()
{
	FGameplayEventData Payload;

	Payload.Instigator = this;

	FGameplayTag PortionTag = FGameplayTag::RequestGameplayTag(FName("Event.Use.HPPortion"));
	Payload.EventTag = PortionTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, PortionTag, Payload);
}

void ABaseHero::UseMPPortion()
{
	FGameplayEventData Payload;

	Payload.Instigator = this;

	FGameplayTag PortionTag = FGameplayTag::RequestGameplayTag(FName("Event.Use.MPPortion"));
	Payload.EventTag = PortionTag;

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, PortionTag, Payload);
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
