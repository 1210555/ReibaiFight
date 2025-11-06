#pragma once

#include "CoreMinimal.h"
#include "AttackComponentBase.h"
#include "NiagaraSystem.h"
#include "AutoSlashAttack.generated.h"

UCLASS(Blueprintable)
class REIBAIFIGHT_API UAutoSlashAttack : public UAttackComponentBase
{
	GENERATED_BODY()

public:
	// 親クラスのPerformAttack関数を上書きします
	virtual void PerformAttack() override;

protected:
	// この攻撃で再生するモンタージュ
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* SlashMontage;

	// 再生したい斬撃エフェクト
	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* SlashEffect;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float defaultSlashDamage = 60.0f;

private:
	//// 最も近い敵を探すヘルパー関数
	//AActor* FindNearestEnemy();

protected:
	// この攻撃が与える基本ダメージ
	UPROPERTY(EditAnywhere, Category = "Combat")
	float BaseDamage = 25.0f;

	//自動攻撃の発動間隔
	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRate = 3.0f;

public:
	// 親クラスのUpgrade関数を上書き
	virtual void Upgrade(const FUpgradeData& UpgradeData) override;
};