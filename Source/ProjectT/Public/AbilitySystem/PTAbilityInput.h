#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPTAbilityInputID : uint8
{
	None,
	Confirm,
	Cancel,
	BasicAttack
};