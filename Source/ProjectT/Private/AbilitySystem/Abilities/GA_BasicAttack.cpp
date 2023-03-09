// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/GA_BasicAttack.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystem/PTAbilitySystemComponent.h"
#include "Characters/BaseHero.h"

UGA_BasicAttack::UGA_BasicAttack()
{

}

void UGA_BasicAttack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(UGA_BasicAttack, SectionName);
}

void UGA_BasicAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	FHitResult HitResult;
	ActorInfo->PlayerController->GetHitResultUnderCursorByChannel(ETraceTypeQuery::TraceTypeQuery1, true, HitResult);
	if (HitResult.GetActor())
	{
		FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(ActorInfo->AvatarActor->GetActorLocation(), HitResult.Location);

		// ActorInfo->AvatarActor->SetActorRotation(FMath::RInterpTo(ActorInfo->AvatarActor->GetActorRotation(), FRotator(ActorInfo->AvatarActor->GetActorRotation().Pitch, Rotation.Yaw, ActorInfo->AvatarActor->GetActorRotation().Roll), GetWorld()->DeltaTimeSeconds, 15.f));

		Cast<ABaseHero>(ActorInfo->AvatarActor)->ServerSetActorRotationToMousePointer(Rotation);
	}

	SectionName = *FString::Printf(TEXT("Attack%d"), Cast<ABaseHero>(ActorInfo->AvatarActor)->CurrentCombo);

}

void UGA_BasicAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}