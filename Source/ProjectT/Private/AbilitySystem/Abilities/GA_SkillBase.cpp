// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_SKillBase.h"
#include "Kismet/KismetMathLibrary.h"

#include "AbilitySystem/PTAbilitySystemComponent.h"
#include "Characters/BaseHero.h"
#include "AbilitySystem/AT_PlayMontageAndWaitForEvent.h"

UGA_SKillBase::UGA_SKillBase()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	FGameplayTag Ability1Tag = FGameplayTag::RequestGameplayTag(FName("State.Attack.Skill1"));
	AbilityTags.AddTag(Ability1Tag);
	ActivationOwnedTags.AddTag(Ability1Tag);

	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Attack")));
}

void UGA_SKillBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	Character = Cast<ABaseHero>(ActorInfo->AvatarActor);

	FHitResult HitResult;
	ActorInfo->PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult);

	FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(ActorInfo->AvatarActor->GetActorLocation(), HitResult.Location);
	Cast<ABaseHero>(ActorInfo->AvatarActor)->ServerSetActorRotationToMousePointer(Rotation);

}

