// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_UpdateTarget.generated.h"

/**
 * 
 */
UCLASS()
class REIBAIFIGHT_API UBTService_UpdateTarget : public UBTService
{
	GENERATED_BODY()
	
private:
	float SerachRadius = 2000.0f;

protected:
	void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector TargetKey;

public:
	UBTService_UpdateTarget();

};
