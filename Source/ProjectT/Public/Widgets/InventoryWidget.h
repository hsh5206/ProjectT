// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"


class ABaseItem;
/**
 * 
 */
UCLASS()
class PROJECTT_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateInventory(TArray<TSubclassOf<ABaseItem>> Inventory);
	void UpdateInventoryWithGridArr();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UInventorySlot> InventorySlotClass;

	TArray<FGridInfo> GridArr;

	/** πŸ¿ŒµÂ */
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* Grid;
};

USTRUCT()
struct FGridInfo
{
	GENERATED_USTRUCT_BODY()

public:
	TSubclassOf<ABaseItem> Item;
	int32 Amount = 1;
};