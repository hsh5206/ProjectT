// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "PTAbilitySystemComponent.h"
#include "PTAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttributeChangeDelegate);

UCLASS()
class PROJECTT_API UPTAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UPTAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue);
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	FAttributeChangeDelegate DeathDelegate;
	
	/** Attributes */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_LV)
	FGameplayAttributeData LV;
	ATTRIBUTE_ACCESSORS(UPTAttributeSet, LV)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_EXP)
	FGameplayAttributeData EXP;
	ATTRIBUTE_ACCESSORS(UPTAttributeSet, EXP)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxEXP)
	FGameplayAttributeData MaxEXP;
	ATTRIBUTE_ACCESSORS(UPTAttributeSet, MaxEXP)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UPTAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UPTAttributeSet, MaxHealth)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Mana)
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UPTAttributeSet, Mana)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_MaxMana)
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UPTAttributeSet, MaxMana)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Vigor)
	FGameplayAttributeData Vigor;	// »ý¸í·Â
	ATTRIBUTE_ACCESSORS(UPTAttributeSet, Vigor)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Power)
	FGameplayAttributeData Power;	// Èû
	ATTRIBUTE_ACCESSORS(UPTAttributeSet, Power)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Agility)
	FGameplayAttributeData Agility;	// ¹ÎÃ¸
	ATTRIBUTE_ACCESSORS(UPTAttributeSet, Agility)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes", ReplicatedUsing = OnRep_Durability)
	FGameplayAttributeData Durability;	// ³»±¸
	ATTRIBUTE_ACCESSORS(UPTAttributeSet, Durability)

	/** Meta Attributes */
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UPTAttributeSet, Damage)

protected:

	UFUNCTION()
	virtual void OnRep_LV(const FGameplayAttributeData& OldLV);
	UFUNCTION()
	virtual void OnRep_EXP(const FGameplayAttributeData& OldEXP);
	UFUNCTION()
	virtual void OnRep_MaxEXP(const FGameplayAttributeData& OldMaxEXP);
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);
	UFUNCTION()
	virtual void OnRep_Mana(const FGameplayAttributeData& OldMana);
	UFUNCTION()
	virtual void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana);
	UFUNCTION()
	virtual void OnRep_Vigor(const FGameplayAttributeData& OldVigor);
	UFUNCTION()
	virtual void OnRep_Power(const FGameplayAttributeData& OldPower);
	UFUNCTION()
	virtual void OnRep_Agility(const FGameplayAttributeData& OldAgility);
	UFUNCTION()
	virtual void OnRep_Durability(const FGameplayAttributeData& OldDurability);

};
