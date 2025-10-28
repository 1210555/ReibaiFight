// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"// �f�[�^�e�[�u�����g���ꍇ�ɕK�v
#include "Containers/Set.h"
#include "ReibaiFightBFL.generated.h"

/**
 * 
 */
class AEnemySpirit;

UCLASS()
class REIBAIFIGHT_API UReibaiFightBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// �w�肵���͈͓��̓G�Ƀ_���[�W��^����ÓI�֐�
	UFUNCTION(BlueprintCallable, Category = "Combat")
	static void ApplyRadialDamage(
		AActor* Attacker,//�U����
		FVector Origin,//�U���̒��S�ʒu
		float Radius,//�U���̃T�C�Y�i���a�j
		float BaseDamage,//�^����_���[�W
		TSet<AActor*>& HitActors
	);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	static AEnemySpirit* GetNearestEnemy(
		const UObject* WorldContextObject, 
		FVector Origin, 
		float Radius
	);
};
