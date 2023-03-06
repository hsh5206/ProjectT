// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"

#include "Items/BaseItem.h"
#include "Widgets/InventoryWidget.h"
#include "Characters/BaseCharacter.h"
#include "Items/BaseWeapon.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (InventoryWidgetClass)
	{
		InventoryWidget = CreateWidget<UInventoryWidget>(GetWorld(), InventoryWidgetClass);
	}
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

void UInventoryComponent::ToggleInventory()
{
	if (InventoryWidget)
	{
		if (InventoryWidget->IsInViewport())
		{
			InventoryWidget->RemoveFromParent();
		}
		else
		{
			InventoryWidget->AddToViewport();
		}
	}
}

void UInventoryComponent::AddItem(ABaseItem* Item)
{
	if (!Character) return;

	Inventory.Add(Item->GetClass());
	InventoryWidget->UpdateInventory(Inventory);
}

void UInventoryComponent::DropItem(TSubclassOf<ABaseItem> Item)
{
	for (int32 i = Inventory.Num()-1; i >= 0; i--)
	{
		if (Inventory[i] == Item)
		{
			Inventory.RemoveAt(i);
			break;
		}
	}
	InventoryWidget->UpdateInventory(Inventory);
	SpawnItem(Item);
}

void UInventoryComponent::SpawnItem(TSubclassOf<ABaseItem> Item)
{
	FVector Location = GetOwner()->GetActorLocation();
	Location += GetOwner()->GetActorForwardVector() * 100.f;
	FRotator Rotation = GetOwner()->GetActorRotation();
	if (GetOwner()->HasAuthority())
	{
		GetWorld()->SpawnActor(Item, &Location, &Rotation);
	}
	else
	{
		ServerSpawnItem(Item, Location, Rotation);
	}
}

void UInventoryComponent::Equip(TSubclassOf<ABaseItem> Item)
{
	for (int32 i = Inventory.Num() - 1; i >= 0; i--)
	{
		if (Inventory[i] == Item)
		{
			Inventory.RemoveAt(i);
			break;
		}
	}
	InventoryWidget->UpdateInventory(Inventory);

	if (GetOwner()->HasAuthority())
	{
		FVector Location = GetOwner()->GetActorLocation();
		Location += GetOwner()->GetActorForwardVector() * 100.f;
		FRotator Rotation = GetOwner()->GetActorRotation();

		EquippingWeapon = Cast<ABaseWeapon>(GetWorld()->SpawnActor(Item, &Location, &Rotation));
		EquippingWeapon->SetOwner(GetOwner());
		EquippingWeapon->Mesh->SetSimulatePhysics(false);
		EquippingWeapon->Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		const USkeletalMeshSocket* Socket = Cast<ACharacter>(GetOwner())->GetMesh()->GetSocketByName(FName("SKT_Weapon"));
		if (Socket)
		{
			Socket->AttachActor(EquippingWeapon, Cast<ACharacter>(GetOwner())->GetMesh());
		}
	}
	else
	{
		ServerEquip(Item);
	}
}

void UInventoryComponent::Unquip(TSubclassOf<ABaseItem> Item)
{
	if (GetOwner()->HasAuthority())
	{
		EquippingWeapon->SetOwner(nullptr);
		EquippingWeapon->Mesh->SetSimulatePhysics(true);
		EquippingWeapon->Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		SpawnItem(EquippingWeapon->GetClass());
		EquippingWeapon->Destroy();
		EquippingWeapon = nullptr;
	}
	else
	{
		ServerUnequip(Item);
	}
}

void UInventoryComponent::ServerUnequip_Implementation(TSubclassOf<ABaseItem> Item)
{
	EquippingWeapon->SetOwner(nullptr);
	EquippingWeapon->Mesh->SetSimulatePhysics(true);
	EquippingWeapon->Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	SpawnItem(EquippingWeapon->GetClass());
	EquippingWeapon->Destroy();
	EquippingWeapon = nullptr;
}

void UInventoryComponent::ServerEquip_Implementation(TSubclassOf<ABaseItem> Item)
{
	FVector Location = GetOwner()->GetActorLocation();
	Location += GetOwner()->GetActorForwardVector() * 100.f;
	FRotator Rotation = GetOwner()->GetActorRotation();

	EquippingWeapon = Cast<ABaseWeapon>(GetWorld()->SpawnActor(Item, &Location, &Rotation));
	EquippingWeapon->SetOwner(GetOwner());
	EquippingWeapon->Mesh->SetSimulatePhysics(false);
	EquippingWeapon->Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	const USkeletalMeshSocket* Socket = Cast<ACharacter>(GetOwner())->GetMesh()->GetSocketByName(FName("SKT_Weapon"));
	if (Socket)
	{
		Socket->AttachActor(EquippingWeapon, Cast<ACharacter>(GetOwner())->GetMesh());
	}
}

void UInventoryComponent::ServerSpawnItem_Implementation(TSubclassOf<ABaseItem> Item, FVector Location, FRotator Rotation)
{
	if (GetOwner()->HasAuthority())
	{
		Cast<ABaseItem>(GetWorld()->SpawnActor(Item, &Location, &Rotation));
		return;
	}
	else
	{
		MulticastSpawnItem(Item, Location, Rotation);
	}
}

void UInventoryComponent::MulticastSpawnItem_Implementation(TSubclassOf<ABaseItem> Item, FVector Location, FRotator Rotation)
{
	Cast<ABaseItem>(GetWorld()->SpawnActor(Item, &Location, &Rotation));
}