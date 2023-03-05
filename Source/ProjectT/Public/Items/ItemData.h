#pragma once

#include "Engine/DataTable.h"
#include "ItemData.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Default UMETA(DisplayName = "Default"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	EIT_Armor UMETA(DisplayName = "Ammor"),
	EIT_Use UMETA(DisplayName = "Use")
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY();

public:
	UPROPERTY(EditDefaultsOnly)
	FName Name;
	UPROPERTY(EditDefaultsOnly)
	FText Description;
	UPROPERTY(EditDefaultsOnly)
	UTexture2D* Icon;
	UPROPERTY(EditDefaultsOnly)
	UStaticMesh* ItemMesh;
	UPROPERTY(EditDefaultsOnly)
	EItemType ItemType;
	UPROPERTY(EditDefaultsOnly)
	bool bCanStack = false;
};

