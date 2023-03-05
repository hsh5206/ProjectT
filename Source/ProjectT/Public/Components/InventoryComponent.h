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
	friend class ABaseCharacter;

protected:
	virtual void BeginPlay() override;

private:	
	class ABaseCharacter* Character;

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
};
