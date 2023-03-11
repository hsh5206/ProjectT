// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseEnemy.h"
#include "Components/WidgetComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Widgets/EnemyHealthBar.h"
#include "AbilitySystem/PTAttributeSet.h"
#include "Characters/BaseHero.h"

ABaseEnemy::ABaseEnemy()
{
	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	if (HealthBarWidget->GetWidget())
	{
		Cast<UEnemyHealthBar>(HealthBarWidget->GetWidget())->SetHPBarPercent(Attributes->GetHealth(), Attributes->GetMaxHealth());
	}
}

void ABaseEnemy::AttackDamageEvent()
{
	FVector Start = GetActorLocation();
	FVector End = GetActorLocation() + GetActorForwardVector() * 200.f;

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
				if (ABaseHero* Target = Cast<ABaseHero>(Hit.GetActor()))
				{
					if (Target->bIsDead) continue;
					FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
					EffectContext.AddHitResult(Hit);
					FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(AttackEffect, 1, EffectContext);
					SpecHandle.Data.Get()->SetSetByCallerMagnitude(FName("SkillDamageMultiplier"), 1.f);
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

void ABaseEnemy::AttackBasic_Implementation()
{
	FGameplayEventData Payload;

	Payload.Instigator = this;
	Payload.EventTag = FGameplayTag::RequestGameplayTag(FName("Event.Enemy.Attack"));

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, Payload.EventTag, Payload);
}