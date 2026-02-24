// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttackComponentBase.h"
#include "AutoShadowBallAttack.generated.h"

class AAttackProjectileBase;
/**
 * 
 */
UCLASS()
class REIBAIFIGHT_API UAutoShadowBallAttack : public UAttackComponentBase
{
	GENERATED_BODY()
	
public:
	// 親クラスのPerformAttack関数を上書きします
	void PerformAttack() override;

	void Upgrade(const FUpgradeData& UpgradeData) override;

	//UPROPERTY(EditDefaultsOnly. BlueprintReadOnly, Category = "Attack")
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attack")
	TSubclassOf<AAttackProjectileBase> ProjectileClass; // スポーンさせるシャドウボールのクラス

	UPROPERTY(EditAnywhere, Category = "Combat")
	float SearchRadius = 3000.0f; // 敵を探す半径

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float CurrentDamage = 20.0f; // 初期ダメージ
};
