// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"

#include "Items/BaseItem.h"
#include "Widgets/InventoryWidget.h"
#include "Characters/BaseCharacter.h"
#include "Items/BaseWeapon.h"
#include "Items/BaseRune.h"

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
	UE_LOG(LogTemp, Warning, TEXT("DropItme called"));
	for (int32 i = Inventory.Num()-1; i >= 0; i--)
	{
		if (Inventory[i] == Item)
		{
			UE_LOG(LogTemp, Warning, TEXT("Removed"));
			Inventory.RemoveAt(i);
			break;
		}
	}
	InventoryWidget->UpdateInventory(Inventory);
	SpawnItem(Item);
}

void UInventoryComponent::SpawnItem(TSubclassOf<ABaseItem> Item)
{
	UE_LOG(LogTemp, Warning, TEXT("Spawned"));

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

	// Weapon
	if (Cast<ABaseWeapon>(Item->GetDefaultObject()))
	{
		if (GetOwner()->HasAuthority())
		{
			FVector Location = GetOwner()->GetActorLocation();
			Location += GetOwner()->GetActorForwardVector() * 100.f;
			FRotator Rotation = GetOwner()->GetActorRotation();

			EquippingWeapon = Cast<ABaseWeapon>(GetWorld()->SpawnActor(Item, &Location, &Rotation));
			EquippingWeapon->SetOwner(GetOwner());
			EquippingWeapon->Mesh->SetSimulatePhysics(false);
			EquippingWeapon->Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

			const USkeletalMeshSocket* Socket = Cast<ACharacter>(GetOwner())->GetMesh()->GetSocketByName(FName("Weapon"));
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

	// Rune
	if (Cast<ABaseRune>(Item->GetDefaultObject()))
	{
		EquippingRunes.Add(Item);
		SetRunesLocation();
	}
	
}

void UInventoryComponent::Unquip(TSubclassOf<ABaseItem> Item)
{
	// Weapon
	if (Cast<ABaseWeapon>(Item->GetDefaultObject()))
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
	
	// Runes
	if (Cast<ABaseRune>(Item->GetDefaultObject()))
	{
		SpawnItem(Item);

		int32 i;
		for (i = EquippingRunes.Num() - 1; i >= 0; i--)
		{
			if (EquippingRunes[i] == Item)
			{
				break;
			}
		}

		EquippingRunes.RemoveAt(i);

		if (i == 0)
		{
			Cast<ABaseCharacter>(GetOwner())->Rune1->SetStaticMesh(nullptr);
		}
		else if (i == 1)
		{
			Cast<ABaseCharacter>(GetOwner())->Rune2->SetStaticMesh(nullptr);
		}
		else if (i == 2)
		{
			Cast<ABaseCharacter>(GetOwner())->Rune3->SetStaticMesh(nullptr);
		}

		SetRunesLocation();
	}
}

void UInventoryComponent::SetRunesLocation()
{

	Cast<ABaseCharacter>(GetOwner())->Rune1->SetStaticMesh(nullptr);
	Cast<ABaseCharacter>(GetOwner())->Rune2->SetStaticMesh(nullptr);
	Cast<ABaseCharacter>(GetOwner())->Rune3->SetStaticMesh(nullptr);

	if (EquippingRunes.Num() >= 1)
	{
		Cast<ABaseCharacter>(GetOwner())->Rune1->SetStaticMesh(EquippingRunes[0].GetDefaultObject()->ItemData.ItemMesh);
	}
	if (EquippingRunes.Num() >= 2)
	{
		Cast<ABaseCharacter>(GetOwner())->Rune2->SetStaticMesh(EquippingRunes[1].GetDefaultObject()->ItemData.ItemMesh);
	}
	if (EquippingRunes.Num() >= 3)
	{
		Cast<ABaseCharacter>(GetOwner())->Rune3->SetStaticMesh(EquippingRunes[2].GetDefaultObject()->ItemData.ItemMesh);
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
		GetWorld()->SpawnActor(Item, &Location, &Rotation);
		return;
	}
	else
	{
		MulticastSpawnItem(Item, Location, Rotation);
	}
}

void UInventoryComponent::MulticastSpawnItem_Implementation(TSubclassOf<ABaseItem> Item, FVector Location, FRotator Rotation)
{
	GetWorld()->SpawnActor(Item, &Location, &Rotation);
}