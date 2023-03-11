// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/PTAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AIPerceptionSystem.h"

#include "Characters/BaseEnemy.h"
#include "Characters/BaseHero.h"

const FName APTAIController::Key_Target(TEXT("Target"));

APTAIController::APTAIController()
{
	BlackboardComponent = Blackboard.Get();

	SetPerceptionComponent(*CreateOptionalDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception")));
	SightConfig = CreateOptionalDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	SightConfig->SightRadius = AISightRadius;
	SightConfig->LoseSightRadius = SightConfig->SightRadius + AILoseSightRadius;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &APTAIController::PerceptionUpdated);
	GetPerceptionComponent()->ConfigureSense(*SightConfig);

}

void APTAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ControlledPawn = InPawn;

	if (ABaseEnemy* Enemy = Cast<ABaseEnemy>(InPawn))
	{
		if (UseBlackboard(Enemy->BBAsset, BlackboardComponent))
		{
			RunBehaviorTree(Enemy->BTAsset);
		}
	}
}

void APTAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

}

void APTAIController::PerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* UpdatedActor : UpdatedActors)
	{
		if (ABaseHero* Hero = Cast<ABaseHero>(UpdatedActor))
		{
			FActorPerceptionBlueprintInfo Info;
			if (GetAIPerceptionComponent()->GetActorsPerception(UpdatedActor, Info))
			{
				for (FAIStimulus Stim : Info.LastSensedStimuli)
				{
					if (Stim.WasSuccessfullySensed())
					{
						Blackboard->SetValueAsObject(Key_Target, UpdatedActor);
						Opponent = UpdatedActor;
					}
					/*else
					{
						Blackboard->SetValueAsObject(Key_Target, nullptr);
						Blackboard->SetValueAsEnum(Key_AIState, (uint8)EAIState::EAIS_Passive);
					}*/
				}
			}
		}
	}
}
