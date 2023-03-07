// Fill out your copyright notice in the Description page of Project Settings.


#include "PTGameInstance.h"

#include "AbilitySystemGlobals.h"

void UPTGameInstance::Init()
{
	Super::Init();

	// 글로벌 데이터 테이블 및 태그를 로드하려면 프로젝트 설정의 일부로 한 번 호출해야 함
	UAbilitySystemGlobals::Get().InitGlobalData();
}