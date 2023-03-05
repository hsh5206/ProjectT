// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/Image.h"

#include "Widgets/InventorySlot.h"
#include "Items/BaseItem.h"

void UInventoryWidget::UpdateInventory(TArray<TSubclassOf<ABaseItem>> Inventory)
{
	Grid->ClearChildren();
	for (auto Item : Inventory)
	{
		UInventorySlot* InventorySlot = CreateWidget<UInventorySlot>(GetWorld(), InventorySlotClass);
		InventorySlot->ItemIcon->SetBrushFromTexture(Item.GetDefaultObject()->Icon);
		InventorySlot->Item = Item;

		int32 row = Grid->GetChildrenCount() / 5;
		int32 colum = Grid->GetChildrenCount() % 5;

		Grid->AddChildToUniformGrid(InventorySlot, row, colum);
	}
}