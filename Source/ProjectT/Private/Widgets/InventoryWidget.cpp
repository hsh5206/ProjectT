// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/InventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "Components/Image.h"

#include "Widgets/InventorySlot.h"
#include "Items/BaseItem.h"

void UInventoryWidget::UpdateInventory(TArray<TSubclassOf<ABaseItem>> Inventory)
{
	Grid->ClearChildren();
	GridArr.Empty();
	for (auto& Item : Inventory)
	{
		if (Item.GetDefaultObject()->ItemData.bCanStack)
		{
			bool bAdded = false;
			for (FGridInfo& GridInfo : GridArr)
			{
				if (GridInfo.Item == Item)
				{
					GridInfo.Amount++;
					bAdded = true;
					break;
				}
			}

			if(!bAdded)
			{
				FGridInfo GridInfo;
				GridInfo.Item = Item;
				GridArr.Add(GridInfo);
			}
		}
		else
		{
			FGridInfo GridInfo;
			GridInfo.Item = Item;
			GridArr.Add(GridInfo);
		}
	}

	UpdateInventoryWithGridArr();
}

void UInventoryWidget::UpdateInventoryWithGridArr()
{
	for(FGridInfo GridInfo : GridArr)
	{
		UInventorySlot* InventorySlot = CreateWidget<UInventorySlot>(GetWorld(), InventorySlotClass);
		InventorySlot->ItemIcon->SetBrushFromTexture(GridInfo.Item.GetDefaultObject()->ItemData.Icon);
		InventorySlot->Item = GridInfo.Item;
		if (GridInfo.Item.GetDefaultObject()->ItemData.bCanStack)
		{
			InventorySlot->SetAmount(GridInfo.Amount);
		}

		int32 row = Grid->GetChildrenCount() / 5;
		int32 colum = Grid->GetChildrenCount() % 5;
		Grid->AddChildToUniformGrid(InventorySlot, row, colum);
	}
}