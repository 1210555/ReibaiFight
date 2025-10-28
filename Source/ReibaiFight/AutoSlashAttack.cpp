#include "AutoSlashAttack.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemySpirit.h" // 敵クラスのヘッダー
#include "NiagaraFunctionLibrary.h"
#include "ReibaiFightBFL.h"


void UAutoSlashAttack::PerformAttack()
{
	// 自分を所有しているアクター（プレイヤーキャラクター）を取得します
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	// 1. 最も近い敵を探します
	AActor* TargetEnemy = UReibaiFightBFL::GetNearestEnemy(
		GetWorld(),//
		OwnerCharacter->GetActorLocation(),//探す原点（プレイヤーの座標）
		3000.0f//探す半径
	);

	// 2. 敵が見つかった場合
	if (TargetEnemy)
	{
		// 3. 敵のほう方向を取得(コメントアウト外せば敵の方向むいちゃう)
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerCharacter->GetActorLocation(), TargetEnemy->GetActorLocation());
		//OwnerCharacter->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));

		// 4. エフェクトを再生します
		if (SlashEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				SlashEffect,
				TargetEnemy->GetActorLocation(), // ← エフェクトを出す（敵の位置）
				LookAtRotation//敵の方向に攻撃発生させたい
			);
		}

		// 5. 攻撃モンタージュを再生します
		UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance && SlashMontage)
		{
			AnimInstance->Montage_Play(SlashMontage);
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

void UAutoSlashAttack::Upgrade(float ModificationValue)
{
	// 以前の自分を呼び出す
	Super::Upgrade(ModificationValue);

	// このコンポーネントが持つDamage変数を増やすなど、具体的な強化処理をここに書く
	BaseDamage += ModificationValue;
	UE_LOG(LogTemp, Warning, TEXT("AutoSlash Attack Upgraded! New Damage: %f"), BaseDamage);
}