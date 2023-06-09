// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/BaseWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "AbilitySystemBlueprintLibrary.h"

#include "Characters/BaseCharacter.h"
#include "AbilitySystem/PTAbilitySystemComponent.h"

ABaseWeapon::ABaseWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("WeaponBox"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	TraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("TraceStart"));
	TraceStart->SetupAttachment(GetRootComponent());
	TraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("TraceEnd"));
	TraceEnd->SetupAttachment(GetRootComponent());
}

void ABaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnBoxOverlap);
}

void ABaseWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	/**
	* Trace
	*/
	const FVector Start = TraceStart->GetComponentLocation();
	const FVector End = TraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(this);
	ActorsToIgnore.Add(Cast<AActor>(GetOwner()));

	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}

	FHitResult BoxHit;

	UKismetSystemLibrary::BoxTraceSingle(
		this,
		Start,
		End,
		FVector(5.f, 5.f, 5.f),
		TraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::ForDuration,
		BoxHit,
		true
	);

	if (BoxHit.GetActor())
	{
		if (ABaseCharacter* Target = Cast<ABaseCharacter>(BoxHit.GetActor()))
		{
			/*FGameplayEventData Payload;

			Payload.Instigator = GetInstigator();
			Payload.EventTag = Cast<ABaseCharacter>(GetOwner())->HitEventTag;
			Payload.Target = Target;
			Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Target);

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), Payload.EventTag, Payload);*/

			ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());

			FGameplayEffectContextHandle EffectContext = OwnerCharacter->AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddHitResult(BoxHit);
			FGameplayEffectSpecHandle SpecHandle = OwnerCharacter->AbilitySystemComponent->MakeOutgoingSpec(AttackEffectToTarget, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				UPTAbilitySystemComponent* TargetASC = Target->AbilitySystemComponent;
				FActiveGameplayEffectHandle ActiveGEHandle = OwnerCharacter->AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
			}
		}
		IgnoreActors.AddUnique(BoxHit.GetActor());
	}
}

void ABaseWeapon::WeaponCollisionEanbled()
{
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}

void ABaseWeapon::WeaponCollisionDisabled()
{
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IgnoreActors.Empty();
}
