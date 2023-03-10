// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Skills/Skill3_SpawnActor.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystemComponent.h"

#include "Characters/BaseCharacter.h"

ASkill3_SpawnActor::ASkill3_SpawnActor()
{
	PrimaryActorTick.bCanEverTick = true;

	DefaultRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	SetRootComponent(DefaultRoot);
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("Box"));
	BoxCollision->SetupAttachment(DefaultRoot);
	Particle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle"));
	Particle->SetupAttachment(DefaultRoot);
}

void ASkill3_SpawnActor::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(4.f);

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::BoxOverlapBegin);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ThisClass::BoxOverlapEnd);
}

void ASkill3_SpawnActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Location = GetActorLocation() + GetActorForwardVector() * MoveSpeed * DeltaTime;
	SetActorLocation(Location);
}


void ASkill3_SpawnActor::BoxOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner()) return;
	if (ABaseCharacter* OwnedCharacter = Cast<ABaseCharacter>(GetOwner()))
	{
		if (ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(OtherActor))
		{
			UPTAbilitySystemComponent* OwnedASC = OwnedCharacter->AbilitySystemComponent;
			UPTAbilitySystemComponent* TargetASC = TargetCharacter->AbilitySystemComponent;

			FGameplayEffectContextHandle ContextHandle = OwnedASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = OwnedASC->MakeOutgoingSpec(Skill3Effect, 0.f, ContextHandle);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = OwnedASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				if (ActiveGEHandle.WasSuccessfullyApplied())
				{
					FMappingEffectHandle MappingEffectHandle;
					MappingEffectHandle.ActorName = *OtherActor->GetName();
					MappingEffectHandle.EffectHandle = ActiveGEHandle;
					RemoveEffectHandles.Add(MappingEffectHandle);
				}
			}
		}
	}
	
}

void ASkill3_SpawnActor::BoxOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(OtherActor))
	{
		UPTAbilitySystemComponent* TargetASC = TargetCharacter->AbilitySystemComponent;
		FName TargetName = *OtherActor->GetName();

		if (RemoveEffectHandles.Num() != 0)
		{
			int32 idx;
			for (idx = 0; idx < RemoveEffectHandles.Num(); idx++)
			{
				FMappingEffectHandle MappingEffectHandle = RemoveEffectHandles[idx];

				if (MappingEffectHandle.ActorName == TargetName)
				{
					if (MappingEffectHandle.EffectHandle.IsValid())
					{
						TargetASC->RemoveActiveGameplayEffect(MappingEffectHandle.EffectHandle);
						RemoveEffectHandles.RemoveAt(idx);
						break;
					}
				}
			}

		}
	}
}


