// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BaseAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "Characters/BaseCharacter.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Character = Cast<ABaseCharacter>(TryGetPawnOwner());
	if (Character)
	{
		Movement = Character->GetCharacterMovement();
	}
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (Movement)
	{
		Speed = Movement->Velocity.Size();
		bIsInAir = Movement->IsFalling();
	}
}
