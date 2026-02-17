// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackComponentBase.h"
#include "NiagaraSystem.h"
#include "ExplosionComponent.generated.h"

/**
 * 
 */
UCLASS()
class REIBAIFIGHT_API UExplosionComponent : public UAttackComponentBase
{
	GENERATED_BODY()
	
public:
	// 親クラスのPerformAttack関数を上書きします
	virtual void PerformAttack() override;

	// 親クラスのUpgrade関数を上書き
	virtual void Upgrade(const FUpgradeData& UpgradeData) override;

protected:
	// 再生したい斬撃エフェクト
	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* ExplosionEffect;

	// この攻撃が与える基本ダメージ
	UPROPERTY(EditAnywhere, Category = "Combat")
	float BaseDamage = 25.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float HitRadius = 300.0f; // この爆発の基本半径

};
