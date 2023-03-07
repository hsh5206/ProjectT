// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/PTGameplayAbility.h"

#include "AbilitySystem/PTAbilitySystemComponent.h"
#include "Characters/BaseCharacter.h"

UPTGameplayAbility::UPTGameplayAbility()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted; // 로컬->서버 실행
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UPTGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	/** OngoingEffectsToJustApplyOnStart Effects 적용 */
	FGameplayEffectContextHandle EffectContext = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	for (auto GameplayEffect : OngoingEffectsToJustApplyOnStart)
	{
		if (!GameplayEffect.Get()) continue;

		if (UAbilitySystemComponent* AbilitySystem = ActorInfo->AbilitySystemComponent.Get())
		{
			FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				if (!ActiveGEHandle.WasSuccessfullyApplied())
				{
					ABILITY_LOG(Log, TEXT("Ability %s failed to apply startup effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
				}
			}
		}
	}

	if (IsInstantiated())
	{
		for (auto GameplayEffect : OngoingEffectsToRemoveOnEnd)
		{
			if (!GameplayEffect.Get()) continue;

			if (UAbilitySystemComponent* AbilitySystem = ActorInfo->AbilitySystemComponent.Get())
			{
				FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(GameplayEffect, 1, EffectContext);
				if (SpecHandle.IsValid())
				{
					FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					if (!ActiveGEHandle.WasSuccessfullyApplied())
					{
						ABILITY_LOG(Log, TEXT("Ability %s failed to apply runtime effect %s"), *GetName(), *GetNameSafe(GameplayEffect));
					}
					else
					{
						RemoveOnEndEffetHandles.Add(ActiveGEHandle);
					}
				}
			}
		}
	}
}

void UPTGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsInstantiated())
	{
		/** RemoveOnEndEffetHandles Effects 해제 */
		for (FActiveGameplayEffectHandle ActiveEffectHandle : RemoveOnEndEffetHandles)
		{
			if (ActiveEffectHandle.IsValid())
			{
				ActorInfo->AbilitySystemComponent->RemoveActiveGameplayEffect(ActiveEffectHandle);
			}
		}

		RemoveOnEndEffetHandles.Empty();
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

ABaseCharacter* UPTGameplayAbility::GetBaseCharacterFromActorInfo() const
{
	return Cast<ABaseCharacter>(GetAvatarActorFromActorInfo());
}
