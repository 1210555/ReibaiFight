// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosionComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "ReibaiFightBFL.h"
#include "Kismet/KismetMathLibrary.h"


void UExplosionComponent::PerformAttack()
{
	// 自分を所有しているアクターを取得。ActorをBaseCharacterにキャスト
	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	// 最も近い敵をTargetEnemyに取得
	AActor* TargetEnemy = UReibaiFightBFL::GetNearestEnemy(
		GetWorld(),//
		OwnerCharacter->GetActorLocation(),//探す原点（プレイヤーの座標）
		3000.0f,//探す半径
		OwnerCharacter,
		OwnerCharacter
	);

	if (TargetEnemy)
	{
		// 敵のほう方向を取得(コメントアウト外せば敵の方向むいちゃう)
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerCharacter->GetActorLocation(), TargetEnemy->GetActorLocation());
		//OwnerCharacter->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));

		// エフェクトを再生します
		if (ExplosionEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				ExplosionEffect,
				TargetEnemy->GetActorLocation(), // ← エフェクトを出す（敵の位置）
				LookAtRotation//敵の方向に攻撃発生させたい
			);
		}


		//今回の攻撃でヒットした敵を記録するための、空のリストを作る
		TSet<AActor*> HitActorsInThisAttack;

		// 当たり判定のロジックは、ライブラリの関数を呼び出すだけでいい
		UReibaiFightBFL::ApplyRadialDamage(
			OwnerCharacter,                 // 攻撃者
			TargetEnemy->GetActorLocation(),// ダメージの中心点（敵の位置）
			HitRadius,                         // ダメージ範囲の半径
			BaseDamage,                     // ダメージ量
			HitActorsInThisAttack           // この攻撃でヒットしたアクターのリスト
		);
	}
}

void UExplosionComponent::Upgrade(const FUpgradeData& UpgradeData)
{
	// 以前の自分を呼び出す
	Super::Upgrade(UpgradeData);

	// このコンポーネントが持つDamage変数を増やすなど、具体的な強化処理をここに書く
	if (UpgradeData.StatModifications.Contains("Damage")) 
	{
		BaseDamage += UpgradeData.StatModifications["Damage"];
	}

	if (UpgradeData.StatModifications.Contains("Rate"))
	{
		// 存在したら、その値を使って頻度を強化
		AttackRate = AttackRate * (1.0f - UpgradeData.StatModifications["Rate"]);
		if (AttackRate < 0.3f) AttackRate = 0.3f;

		//タイマーを新しい間隔で再起動
		ActivateAttack();
	}
	if (UpgradeData.StatModifications.Contains("Size"))
	{
		HitRadius = HitRadius * (1 + UpgradeData.StatModifications["Size"]);
	}
}