// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/BaseItem.h"

#include "Components/SphereComponent.h"
#include "Characters/BaseHero.h"
#include "Components/WidgetComponent.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	bReplicates = true;
	SetReplicateMovement(true);
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereOverlapEnd);
	}

	if (ItemData.ItemMesh)
	{
		Mesh->SetStaticMesh(ItemData.ItemMesh);
	}
}

void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABaseItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ABaseHero* Character = Cast<ABaseHero>(OtherActor))
	{
		if (Character->IsLocallyControlled())
		{
			if (Character->OverlappingItems.Num() == 0)
			{
				Character->PickupWidget->AddToViewport();
			}
		}
		Character->OverlappingItems.Add(this);
	}
}

void ABaseItem::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (ABaseHero* Character = Cast<ABaseHero>(OtherActor))
	{
		Character->OverlappingItems.Remove(this);
		if (Character->IsLocallyControlled())
		{
			if (Character->OverlappingItems.Num() == 0)
			{
				Character->PickupWidget->RemoveFromParent();
			}
		}
	}
}