// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class ABaseItem;
class UInventoryWidget;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	friend class ABaseHero;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

private:	
	class ABaseHero* Character;

public:
	UPROPERTY()
	UInventoryWidget* InventoryWidget;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;
	void ToggleInventory();

	UPROPERTY(VisibleAnywhere)
	TArray<TSubclassOf<ABaseItem>> Inventory;

	void AddItem(ABaseItem* Item);
	UFUNCTION(BlueprintCallable)
	void DropItem(TSubclassOf<ABaseItem> Item);
	UFUNCTION(Server, Reliable)
	void ServerSpawnItem(TSubclassOf<ABaseItem> Item, FVector Location, FRotator Rotation);
	UFUNCTION(BlueprintCallable)
	void SpawnItem(TSubclassOf<ABaseItem> Item);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpawnItem(TSubclassOf<ABaseItem> Item, FVector Location, FRotator Rotation);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	class ABaseWeapon* EquippingWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<TSubclassOf<ABaseItem>> EquippingRunes;

	UFUNCTION(BlueprintCallable)
	void Equip(TSubclassOf<ABaseItem> Item);
	UFUNCTION(Server, Reliable)
	void ServerEquip(TSubclassOf<ABaseItem> Item);
	UFUNCTION(BlueprintCallable)
	void Unquip(TSubclassOf<ABaseItem> Item);
	UFUNCTION(Server, Reliable)
	void ServerUnequip(TSubclassOf<ABaseItem> Item);
	void SetRunesLocation();
	UFUNCTION(Server, Reliable)
	void ServerSetRunesLocation();

};
