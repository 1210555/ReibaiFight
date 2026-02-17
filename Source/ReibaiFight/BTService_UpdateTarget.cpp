// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_UpdateTarget.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "ReibaiFightBFL.h"
#include "BaseCharacter.h"

UBTService_UpdateTarget::UBTService_UpdateTarget()
{
	NodeName = TEXT("Update Target");
}

void UBTService_UpdateTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	AAIController* AIController = OwnerComp.GetAIOwner();
	ABaseCharacter* MyPawn = AIController ? Cast<ABaseCharacter>(AIController->GetPawn()) : nullptr;

	if (MyPawn)
	{
		ABaseCharacter* FoundTarget = UReibaiFightBFL::GetNearestEnemy(
			this,
			MyPawn->GetActorLocation(),
			SerachRadius,
			MyPawn,
			MyPawn
		);

		if (FoundTarget)
		{
			//TargetKey‚ÉFoundTarget‚ðƒZƒbƒg
			BlackboardComp->SetValueAsObject(TargetKey.SelectedKeyName, FoundTarget);
		}
		else
		{
			BlackboardComp->ClearValue(TargetKey.SelectedKeyName);
		}
	}
}
