// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventorySlot.h"

#include "Components/TextBlock.h"

void UInventorySlot::SetItemValues()
{
}

void UInventorySlot::SetAmount(int32 amount)
{
	Amount->SetText(FText::FromString(FString::Printf(TEXT("%d"), amount)));
}