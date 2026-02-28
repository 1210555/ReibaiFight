// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackComponentBase.h"
#include "ThunderAttack.generated.h"

/**
 * 
 */
UCLASS()
class REIBAIFIGHT_API UThunderAttack : public UAttackComponentBase
{
	GENERATED_BODY()

public:
	// 親クラスのPerformAttack関数を上書き
	void PerformAttack() override;

protected:

	// 再生したい電撃エフェクト
	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* ThunderEffect;

	// この攻撃が与える基本ダメージ
	UPROPERTY(EditAnywhere, Category = "Combat")
	float BaseDamage = 25.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float BaseRadius = 150.0f;

	int32 CurrentBurstCount = 0; // 現在の連射数

	int32 MaxBurstCount = 1; // 最大連射数。(デフォルトは１)

	float BurstInterval = 0.3; // 連射の間隔（ミリ秒）

	void FireSingleThunder();

public:
	// 親クラスのUpgrade関数を上書き
	void Upgrade(const FUpgradeData& UpgradeData) override;

	//void ActivateAttack() override;

};
