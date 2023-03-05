// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/Image.h"

#include "Widgets/InventorySlot.h"
#include "Items/BaseItem.h"

void UInventoryWidget::AddSlot(ABaseItem* Item)
{
	if (InventorySlotClass)
	{
		UInventorySlot* InventorySlot = CreateWidget<UInventorySlot>(GetWorld(), InventorySlotClass);
		//InventorySlot->ItemIcon->SetBrushFromTexture(Item->Icon);
		InventorySlot->Item = Item->GetClass();

		int32 row = Grid->GetChildrenCount() / 5;
		int32 colum = Grid->GetChildrenCount() % 5;

		Grid->AddChildToUniformGrid(InventorySlot, row, colum);
	}
} 

void UInventoryWidget::RemoveSlot(ABaseItem* Item)
{
}
