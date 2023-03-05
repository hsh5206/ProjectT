// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlot.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTT_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UImage* ItemIcon;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* Amount;

	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<class ABaseItem> Item;

	void SetItemValues();
	void SetAmount(int32 amount);
};
