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
	void AddSlot(ABaseItem* Item);
	void RemoveSlot(ABaseItem* Item);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class TSubclassOf<class UInventorySlot> InventorySlotClass;

	/** πŸ¿ŒµÂ */
	UPROPERTY(meta = (BindWidget))
	class UUniformGridPanel* Grid;
};
