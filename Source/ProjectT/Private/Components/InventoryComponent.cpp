// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent.h"

#include "Items/BaseItem.h"
#include "Widgets/InventoryWidget.h"
#include "Characters/BaseCharacter.h"

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
	FVector Location = GetOwner()->GetActorLocation();
	Location += GetOwner()->GetActorForwardVector() * 100.f;
	FRotator Rotation = GetOwner()->GetActorRotation();
	ServerSpawnItem(Item, Location, Rotation);
}

void UInventoryComponent::ServerSpawnItem_Implementation(TSubclassOf<ABaseItem> Item, FVector Location, FRotator Rotation)
{
	GetWorld()->SpawnActor(Item, &Location, &Rotation);
}
