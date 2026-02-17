// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_CalculateDistance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

UBTService_CalculateDistance::UBTService_CalculateDistance()
{
	// サービスの名前を設定
	NodeName = TEXT("Calculate Distance to Player");
}

void UBTService_CalculateDistance::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	// ブラックボードコンポーネントを取得
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return;

	//
	AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (TargetActor)
	{
		// AIのコントローラーを取得
		AAIController* OwnerController = OwnerComp.GetAIOwner();

		// AIの所有者を取得
		APawn* AIPawn = OwnerController ? OwnerController->GetPawn() : nullptr;
		
		if (AIPawn)
		{
			// AIとターゲットアクター間の距離を計算
			float DistanceToTarget = FVector::Dist(AIPawn->GetActorLocation(), TargetActor->GetActorLocation());
			// ブラックボードに距離を設定
			BlackboardComp->SetValueAsFloat(DistanceResultKey.SelectedKeyName, DistanceToTarget);
		}
	}
	else 
	{
		BlackboardComp->SetValueAsFloat(DistanceResultKey.SelectedKeyName, 0.0f);
	}
	
}