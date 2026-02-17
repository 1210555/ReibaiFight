#include "AutoSlashAttack.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
//#include "EnemySpirit.h" // 敵クラスのヘッダー
#include "NiagaraFunctionLibrary.h"
#include "ReibaiFightBFL.h"

void UAutoSlashAttack::PerformAttack()
{
	// 自分を所有しているアクター（プレイヤーキャラクター）を取得します
	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
	if (!OwnerCharacter) return;

	//最も近い敵を探す
	//BFLのGetNearestEnemy関数を使用
	//それをActor型の箱のポインタに格納
	AActor* TargetEnemy = UReibaiFightBFL::GetNearestEnemy(
		GetWorld(),//
		OwnerCharacter->GetActorLocation(),//探す原点（プレイヤーの座標）
		3000.0f,//探す半径
		OwnerCharacter,
		OwnerCharacter
	);

	//最も近い敵が見つかったとき
	if (TargetEnemy)
	{
		//敵の方向を取得(コメントアウト外せば敵の方向むいちゃう)
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerCharacter->GetActorLocation(), TargetEnemy->GetActorLocation());
		//OwnerCharacter->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));

		//エフェクトを再生します
		if (SlashEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				SlashEffect,
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
			150.0f,                         // ダメージ範囲の半径
			BaseDamage,                     // ダメージ量
			HitActorsInThisAttack           // この攻撃でヒットしたアクターのリスト
		);
	}
}

void UAutoSlashAttack::Upgrade(const FUpgradeData& UpgradeData)
{
	// 以前の自分を呼び出す
	Super::Upgrade(UpgradeData);

	if (UpgradeData.StatModifications.Contains("Damage"))
	{
		// 存在したら、その値を使ってダメージを強化
		BaseDamage += UpgradeData.StatModifications["Damage"];
		UE_LOG(LogTemp, Warning, TEXT("Slash Damage Upgraded! New: %f"), BaseDamage);
	}

	if (UpgradeData.StatModifications.Contains("Rate"))
	{
		// 存在したら、その値を使って頻度を強化
		AttackRate = AttackRate * (1.0f - UpgradeData.StatModifications["Rate"]);
		if (AttackRate < 0.3f) AttackRate = 0.3f;
		UE_LOG(LogTemp, Warning, TEXT("Slash Rate Upgraded! New: %f"), AttackRate);

		//タイマーを新しい間隔で再起動
		ActivateAttack();
	}

	//ナイアガラに変更があればそれに変更する
	if (UpgradeData.NewNiagaraEffect != nullptr)
	{
		// ...このコンポーネントが使うエフェクト(SlashEffect)を、その新しいエフェクトに差し替える
		this->SlashEffect = UpgradeData.NewNiagaraEffect;
		UE_LOG(LogTemp, Warning, TEXT("Attack Effect Upgraded! %s"), *UpgradeData.NewNiagaraEffect->GetName());
	}

}