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
	void PerformAttack() override;

protected:

	// 再生したい斬撃エフェクト
	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* SlashEffect;

	// この攻撃が与える基本ダメージ
	UPROPERTY(EditAnywhere, Category = "Combat")
	float BaseDamage = 25.0f;

public:
	// 親クラスのUpgrade関数を上書き
	virtual void Upgrade(const FUpgradeData& UpgradeData) override;
};