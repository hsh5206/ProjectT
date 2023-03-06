// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/BaseWeapon.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Characters/BaseCharacter.h"

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

	int32 TotalDamage = Damage;

	if (BoxHit.GetActor())
	{
		if (BoxHit.GetActor()->ActorHasTag(FName("Enemy")))
		{
			UGameplayStatics::ApplyDamage(BoxHit.GetActor(),
				TotalDamage,
				GetInstigator()->GetController(),
				this,
				UDamageType::StaticClass()
			);

			/*IHitInterface* HitInterface = Cast<IHitInterface>(BoxHit.GetActor());
			if (HitInterface)
			{
				HitInterface->Execute_GetHit(BoxHit.GetActor(), BoxHit.ImpactPoint);
			}
			IgnoreActors.AddUnique(BoxHit.GetActor());*/
		}
	}
}