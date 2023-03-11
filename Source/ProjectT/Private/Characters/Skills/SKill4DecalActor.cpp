// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Skills/SKill4DecalActor.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"

#include "Characters/BaseEnemy.h"
#include "AbilitySystem/PTAbilitySystemComponent.h"

ASKill4DecalActor::ASKill4DecalActor()
{
	PrimaryActorTick.bCanEverTick = false;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	SetRootComponent(DefaultRoot);
	Decal = CreateDefaultSubobject<UDecalComponent>(TEXT("Decal"));
	Decal->SetupAttachment(DefaultRoot);
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("Box"));
	SphereCollision->SetupAttachment(Decal);
}

void ASKill4DecalActor::BeginPlay()
{
	Super::BeginPlay();
	
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::SphereOverlapBegin);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ThisClass::SphereOverlapEnd);
}

void ASKill4DecalActor::SphereOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(OtherActor))
	{
		OverlappingEnemies.AddUnique(Enemy);
	}
}

void ASKill4DecalActor::SphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(OtherActor))
	{
		OverlappingEnemies.Remove(Enemy);
	}
}

void ASKill4DecalActor::DamageToEnemies()
{
	if (OverlappingEnemies.Num() != 0)
	{
		for (auto Enemy : OverlappingEnemies)
		{
			FGameplayEffectContextHandle EffectContext = Cast<ABaseCharacter>(GetOwner())->AbilitySystemComponent->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = Cast<ABaseCharacter>(GetOwner())->AbilitySystemComponent->MakeOutgoingSpec(Skill4Effect, 1, EffectContext);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(FName("SkillDamageMultiplier"), 1.5f);
			if (SpecHandle.IsValid())
			{
				UPTAbilitySystemComponent* TargetASC = Enemy->AbilitySystemComponent;
				FActiveGameplayEffectHandle ActiveGEHandle = Cast<ABaseCharacter>(GetOwner())->AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
		}
	}
}

void ASKill4DecalActor::FinalDamageToEnemies()
{
	if (OverlappingEnemies.Num() != 0)
	{
		for (auto Enemy : OverlappingEnemies)
		{
			FGameplayEffectContextHandle EffectContext = Cast<ABaseCharacter>(GetOwner())->AbilitySystemComponent->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = Cast<ABaseCharacter>(GetOwner())->AbilitySystemComponent->MakeOutgoingSpec(Skill4Effect, 1, EffectContext);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(FName("SkillDamageMultiplier"), 2.f);
			if (SpecHandle.IsValid())
			{
				UPTAbilitySystemComponent* TargetASC = Enemy->AbilitySystemComponent;
				FActiveGameplayEffectHandle ActiveGEHandle = Cast<ABaseCharacter>(GetOwner())->AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
		}
	}
}

