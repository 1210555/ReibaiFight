// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_CalculateDistance.generated.h"

/**
 * 
 */
UCLASS()
class REIBAIFIGHT_API UBTService_CalculateDistance : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTService_CalculateDistance();

protected:
	void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	//誰との距離を測るか。エディタでTargetActor,LeaderActorを選ぶ
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetActorKey;

	//結果をどこに入れるか。エディタでDistanceToTarget,DistanceToLeaderを選ぶ
	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector DistanceResultKey;
};
