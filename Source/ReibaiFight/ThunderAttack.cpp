// Fill out your copyright notice in the Description page of Project Settings.


#include "ThunderAttack.h"
#include "ReibaiFightBFL.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "BaseCharacter.h"

void UThunderAttack::PerformAttack()
{
	UE_LOG(LogTemp, Warning, TEXT("ThunderAttack PerformAttack called!"));
	CurrentBurstCount = MaxBurstCount; // 連射数をリセット
	GetWorld()->GetTimerManager().SetTimer(
		BurstTimerHandle, // タイマーハンドル
		this,             // タイマーの対象オブジェクト
		&UThunderAttack::FireSingleThunder, // 呼び出す関数
		0.3f,       // 間隔（秒）
		true,              // 繰り返すかどうか
		0.0f               // 最初の遅延（秒）
	);
}

void UThunderAttack::Upgrade(const FUpgradeData& UpgradeData)
{
	// 以前の自分を呼び出す
	Super::Upgrade(UpgradeData);
	UE_LOG(LogTemp, Warning, TEXT("Upgrading Thunder Attack with new data!"));

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

	if(UpgradeData.StatModifications.Contains("BurstCount"))
	{
		MaxBurstCount += UpgradeData.StatModifications["BurstCount"];
		UE_LOG(LogTemp, Warning, TEXT("Burst Count Upgraded! New: %d"), MaxBurstCount);
	}

	//ナイアガラに変更があればそれに変更する
	if (UpgradeData.NewNiagaraEffect != nullptr)
	{
		// ...このコンポーネントが使うエフェクト(SlashEffect)を、その新しいエフェクトに差し替える
		this->ThunderEffect = UpgradeData.NewNiagaraEffect;
		UE_LOG(LogTemp, Warning, TEXT("Attack Effect Upgraded! %s"), *UpgradeData.NewNiagaraEffect->GetName());
	}
}

//void UThunderAttack::ActivateAttack()
//{
//	// まずは親クラスのActivateAttackを呼び出す
//	Super::ActivateAttack();
//
//	float BurstInterval = 0.3f; // 連射数に応じて間隔を調整
//	int32 CurrentBurstCount = BurstCount; // 現在の連射数を保存
//
//	if (CurrentBurstCount > 0) {
//
//		// 既にタイマーが動いている場合は一度クリアする
//		GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
//		//バースト用のタイマーを開始する
//		GetWorld()->GetTimerManager().SetTimer(
//			BurstTimerHandle, // タイマーハンドル
//			this,             // タイマーの対象オブジェクト
//			&UThunderAttack::PerformAttack, // 呼び出す関数
//			BurstInterval,       // 間隔（秒）
//			true,              // 繰り返すかどうか
//			0.0f               // 最初の遅延（秒）
//		);
//		CurrentBurstCount--; // 連射数を減らす
//	}
//}

//BurstIntervalの間隔で、PerformAttack関数を呼び出すタイマーが動いている状態で、PerformAttack関数の中でFireSingleThunder関数を呼び出すようにする
void UThunderAttack::FireSingleThunder()
{
	UE_LOG(LogTemp, Warning, TEXT("FireSingleThunder called! Remaining Burst Count: %d"), CurrentBurstCount);
	if (CurrentBurstCount > 0) {
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
			if (ThunderEffect)
			{
				UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					ThunderEffect,
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
				BaseRadius,                         // ダメージ範囲の半径
				BaseDamage,                     // ダメージ量
				HitActorsInThisAttack           // この攻撃でヒットしたアクターのリスト
			);
		}
		CurrentBurstCount--; // 連射数を減らす
	}
	else {
		// 連射数が0になったら、タイマーをクリアして終了
		GetWorld()->GetTimerManager().ClearTimer(BurstTimerHandle);
	}
}