// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/BaseItem.h"

#include "Components/SphereComponent.h"
#include "Characters/BaseCharacter.h"
#include "Components/WidgetComponent.h"

ABaseItem::ABaseItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(Mesh);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(GetRootComponent());

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(GetRootComponent());
	PickupWidget->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupWidget->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	bReplicates = true;
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
	
	if (HasAuthority())
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlap);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereOverlapEnd);
	}
}

void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void ABaseItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap Begin"));
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(OtherActor))
	{
		//PickupWidget->SetVisibility(true);
		Character->OverlappingItem = this;
		if (Character->IsLocallyControlled())
		{
			PickupWidget->SetVisibility(true);
		}
	}
}

void ABaseItem::OnSphereOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlap End"));
	if (ABaseCharacter* Character = Cast<ABaseCharacter>(OtherActor))
	{
		Character->OverlappingItem = nullptr;
		if (Character->IsLocallyControlled())
		{
			PickupWidget->SetVisibility(false);
		}
	}
}

void ABaseItem::SetPickupWidgetVisibility(bool bValue)
{
	PickupWidget->SetVisibility(bValue);
}