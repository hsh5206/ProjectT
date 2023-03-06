// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Components/SphereComponent.h"

#include "Items/BaseItem.h"
#include "Widgets/InventoryWidget.h"
#include "Characters/BaseCharacter.h"
#include "Items/BaseWeapon.h"
#include "Items/BaseArmor.h"

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

	// Armor
	if (ABaseArmor* Armor = Cast<ABaseArmor>(Item->GetDefaultObject()))
	{
		FVector Location = GetOwner()->GetActorLocation();
		Location += GetOwner()->GetActorForwardVector() * 100.f;
		FRotator Rotation = GetOwner()->GetActorRotation();

		if (Armor->ItemData.ItemType == EItemType::EIT_ArmorHead)
		{
			EquippingArmorHead = Cast<ABaseArmor>(GetWorld()->SpawnActor(Item, &Location, &Rotation));
			const USkeletalMeshSocket* Socket = Cast<ACharacter>(GetOwner())->GetMesh()->GetSocketByName(FName("Head"));
			if (Socket)
			{
				Socket->AttachActor(EquippingArmorHead, Cast<ACharacter>(GetOwner())->GetMesh());
			}
			EquippingArmorHead->SetOwner(GetOwner());
			EquippingArmorHead->Mesh->SetSimulatePhysics(false);
			EquippingArmorHead->Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else if (Armor->ItemData.ItemType == EItemType::EIT_ArmorTop)
		{
			EquippingArmorTop = Cast<ABaseArmor>(GetWorld()->SpawnActor(Item, &Location, &Rotation));
			const USkeletalMeshSocket* Socket = Cast<ACharacter>(GetOwner())->GetMesh()->GetSocketByName(FName("Top"));
			if (Socket)
			{
				Socket->AttachActor(EquippingArmorTop, Cast<ACharacter>(GetOwner())->GetMesh());
			}
			EquippingArmorTop->SetOwner(GetOwner());
			EquippingArmorTop->Mesh->SetSimulatePhysics(false);
			EquippingArmorTop->Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		else if (Armor->ItemData.ItemType == EItemType::EIT_ArmorBottom)
		{
			EquippingArmorBottom = Cast<ABaseArmor>(GetWorld()->SpawnActor(Item, &Location, &Rotation));
			const USkeletalMeshSocket* Socket = Cast<ACharacter>(GetOwner())->GetMesh()->GetSocketByName(FName("Bottom"));
			if (Socket)
			{
				Socket->AttachActor(EquippingArmorTop, Cast<ACharacter>(GetOwner())->GetMesh());
			}
			EquippingArmorBottom->SetOwner(GetOwner());
			EquippingArmorBottom->Mesh->SetSimulatePhysics(false);
			EquippingArmorBottom->Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		// 추후구현
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
	
	// Armor
	if (ABaseArmor* Armor = Cast<ABaseArmor>(Item->GetDefaultObject()))
	{
		if (Armor->ItemData.ItemType == EItemType::EIT_ArmorHead)
		{
			EquippingArmorHead->SetOwner(nullptr);
			EquippingArmorHead->Mesh->SetSimulatePhysics(true);
			EquippingArmorHead->Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			SpawnItem(EquippingArmorHead->GetClass());
			EquippingArmorHead->Destroy();
			EquippingArmorHead = nullptr;
		}
		else if (Armor->ItemData.ItemType == EItemType::EIT_ArmorTop)
		{
			EquippingArmorTop ->SetOwner(nullptr);
			EquippingArmorTop->Mesh->SetSimulatePhysics(true);
			EquippingArmorTop->Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			SpawnItem(EquippingArmorTop->GetClass());
			EquippingArmorHead->Destroy();
			EquippingArmorTop = nullptr;
		}
		else if (Armor->ItemData.ItemType == EItemType::EIT_ArmorBottom)
		{
			EquippingArmorBottom->SetOwner(nullptr);
			EquippingArmorBottom->Mesh->SetSimulatePhysics(true);
			EquippingArmorBottom->Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
			SpawnItem(EquippingArmorBottom->GetClass());
			EquippingArmorHead->Destroy();
			EquippingArmorBottom = nullptr;
		}
		

		// 추후구현
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