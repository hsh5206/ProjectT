// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseCharacter.h"

#include "Components/CapsuleComponent.h"

#include "AbilitySystem/PTAbilitySystemComponent.h"
#include "AbilitySystem/PTAttributeSet.h"
#include "AbilitySystem/PTGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = false;
	SetReplicates(true);

	AbilitySystemComponent = CreateDefaultSubobject<UPTAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	Attributes = CreateDefaultSubobject<UPTAttributeSet>(TEXT("AttributeSet"));
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (Attributes)
	{
		Attributes->DeathDelegate.AddDynamic(this, &ABaseCharacter::Death);
	}
}

void ABaseCharacter::Death()
{
	UE_LOG(LogTemp, Warning, TEXT("Death"));

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ServerPlayMontage(DeathMontage);
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

/**
*
*	GAS
*
*/
UAbilitySystemComponent* ABaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


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
