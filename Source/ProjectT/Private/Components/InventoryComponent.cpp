// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

#include "Items/BaseItem.h"
#include "Widgets/InventoryWidget.h"
#include "Characters/BaseHero.h"
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

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryComponent, EquippingRunes);
	DOREPLIFETIME(UInventoryComponent, EquippingWeapon);
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
		if (GetOwner()->HasAuthority())
		{
			EquippingRunes.Add(Item);
			SetRunesLocation();
		}
		else
		{
			ServerEquip(Item);
		}
	}
	
}

void UInventoryComponent::ServerEquip_Implementation(TSubclassOf<ABaseItem> Item)
{
	if (Cast<ABaseWeapon>(Item->GetDefaultObject()))
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
	else if (Cast<ABaseRune>(Item->GetDefaultObject()))
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
		if (GetOwner()->HasAuthority())
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
				Cast<ABaseHero>(GetOwner())->Rune1->SetStaticMesh(nullptr);
			}
			else if (i == 1)
			{
				Cast<ABaseHero>(GetOwner())->Rune2->SetStaticMesh(nullptr);
			}
			else if (i == 2)
			{
				Cast<ABaseHero>(GetOwner())->Rune3->SetStaticMesh(nullptr);
			}

			SetRunesLocation();
		}
		else
		{
			ServerUnequip(Item);
		}
	}
}

void UInventoryComponent::ServerUnequip_Implementation(TSubclassOf<ABaseItem> Item)
{
	if (Cast<ABaseWeapon>(Item->GetDefaultObject()))
	{
		EquippingWeapon->SetOwner(nullptr);
		EquippingWeapon->Mesh->SetSimulatePhysics(true);
		EquippingWeapon->Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		SpawnItem(EquippingWeapon->GetClass());
		EquippingWeapon->Destroy();
		EquippingWeapon = nullptr;
	}
	else if (Cast<ABaseRune>(Item->GetDefaultObject()))
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
			Cast<ABaseHero>(GetOwner())->Rune1->SetStaticMesh(nullptr);
		}
		else if (i == 1)
		{
			Cast<ABaseHero>(GetOwner())->Rune2->SetStaticMesh(nullptr);
		}
		else if (i == 2)
		{
			Cast<ABaseHero>(GetOwner())->Rune3->SetStaticMesh(nullptr);
		}

		SetRunesLocation();
	}
}

void UInventoryComponent::SetRunesLocation()
{
	if (GetOwner()->HasAuthority())
	{
		Cast<ABaseHero>(GetOwner())->Rune1->SetStaticMesh(nullptr);
		Cast<ABaseHero>(GetOwner())->Rune2->SetStaticMesh(nullptr);
		Cast<ABaseHero>(GetOwner())->Rune3->SetStaticMesh(nullptr);

		if (EquippingRunes.Num() >= 1)
		{
			Cast<ABaseHero>(GetOwner())->Rune1->SetStaticMesh(EquippingRunes[0].GetDefaultObject()->ItemData.ItemMesh);
		}
		if (EquippingRunes.Num() >= 2)
		{
			Cast<ABaseHero>(GetOwner())->Rune2->SetStaticMesh(EquippingRunes[1].GetDefaultObject()->ItemData.ItemMesh);
		}
		if (EquippingRunes.Num() >= 3)
		{
			Cast<ABaseHero>(GetOwner())->Rune3->SetStaticMesh(EquippingRunes[2].GetDefaultObject()->ItemData.ItemMesh);
		}
	}
	else
	{
		ServerSetRunesLocation();
	}
}

void UInventoryComponent::ServerSetRunesLocation_Implementation()
{
	Cast<ABaseHero>(GetOwner())->Rune1->SetStaticMesh(nullptr);
	Cast<ABaseHero>(GetOwner())->Rune2->SetStaticMesh(nullptr);
	Cast<ABaseHero>(GetOwner())->Rune3->SetStaticMesh(nullptr);

	if (EquippingRunes.Num() >= 1)
	{
		Cast<ABaseHero>(GetOwner())->Rune1->SetStaticMesh(EquippingRunes[0].GetDefaultObject()->ItemData.ItemMesh);
	}
	if (EquippingRunes.Num() >= 2)
	{
		Cast<ABaseHero>(GetOwner())->Rune2->SetStaticMesh(EquippingRunes[1].GetDefaultObject()->ItemData.ItemMesh);
	}
	if (EquippingRunes.Num() >= 3)
	{
		Cast<ABaseHero>(GetOwner())->Rune3->SetStaticMesh(EquippingRunes[2].GetDefaultObject()->ItemData.ItemMesh);
	}
}