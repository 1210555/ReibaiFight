// Fill out your copyright notice in the Description page of Project Settings.


#include "AutoShadowBallAttack.h"
#include "ReibaiFightBFL.h"
#include "BaseEnemy.h"
#include "BaseCharacter.h"
#include "AttackProjectileBase.h" //このクラスをスポーンさせる
#include "GameFramework/ProjectileMovementComponent.h"
#include "ReibaiFightCharacter.h" 


void UAutoShadowBallAttack::PerformAttack()
{
	ABaseCharacter* OwnerCharacter = Cast<ABaseCharacter>(GetOwner());//<ReibaiFIghtCharacterでないのはこれを味方になった敵なども使えるようにするため。
	AReibaiFightCharacter* PlayerCharacter = Cast<AReibaiFightCharacter>(OwnerCharacter);
	if (!OwnerCharacter || !PlayerCharacter) return;

	ABaseCharacter* TargetEnemy = UReibaiFightBFL::GetNearestEnemy(
		GetWorld(),
		OwnerCharacter->GetActorLocation(),
		SearchRadius,
		PlayerCharacter,
		PlayerCharacter
	);

	if (TargetEnemy) {

		//敵と自分の距離を計算して、.GetSafeNormalで単位ベクトルにして扱う
		FVector EnemyLocation = TargetEnemy->GetActorLocation();
		FVector PlayerLocation = PlayerCharacter->GetActorLocation();
		FVector DirectionToEnemy = (EnemyLocation - PlayerLocation).GetSafeNormal();

		FVector SpawnLocation = PlayerLocation + DirectionToEnemy * 200.0f; //取得した方向から200離れた場所にスポーン
		FRotator SpawnRotation = DirectionToEnemy.Rotation(); // 方向ベクトルから回転を作成

		if (ProjectileClass) {
			AAttackProjectileBase* SpawnedBall = GetWorld()->SpawnActor<AAttackProjectileBase>(ProjectileClass, SpawnLocation, SpawnRotation);
			if (SpawnedBall) {
				// ホーミングのターゲットを設定
				SpawnedBall->ShooterCharacter = OwnerCharacter;//誰が撃ったかに
				SpawnedBall->TargetActor = TargetEnemy;
				SpawnedBall->DamageAmount = CurrentDamage; // ダメージ量を設定（必要に応じて変更）

				if (SpawnedBall->ProjectileMovement) {
					SpawnedBall->ProjectileMovement->HomingTargetComponent = TargetEnemy->GetRootComponent(); // ホーミングのターゲットを設定
					UE_LOG(LogTemp, Warning, TEXT("Homing target set to %s"), *TargetEnemy->GetName());
				}
				else {
					UE_LOG(LogTemp, Warning, TEXT("ProjectileMovement component not found on spawned ball!"));
				}
			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Shadow Ball!"));
			}
			
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("ProjectileClass not set for AutoShadowBallAttack!"));
		}
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("No target found for AutoShadowBallAttack!"));
	}

	UE_LOG(LogTemp, Warning, TEXT("AutoShadowBallAttack performed!"));
}

void UAutoShadowBallAttack::Upgrade(const FUpgradeData& UpgradeData)
{
	// 以前の自分を呼び出す
	Super::Upgrade(UpgradeData);

	UE_LOG(LogTemp, Warning, TEXT("AutoShadowBallAttack upgraded!"));

	//ダメージ増加
	if(UpgradeData.StatModifications.Contains("Damage"))
	{
		CurrentDamage += UpgradeData.StatModifications["Damage"];
	}
	
	//発生間隔の短縮
	if (UpgradeData.StatModifications.Contains("Rate"))
	{
		AttackRate *= 1 - UpgradeData.StatModifications["Rate"];
		UE_LOG(LogTemp, Warning, TEXT("New Attack Rate is %f"), AttackRate);

		ActivateAttack(); // 攻撃間隔を更新
	}
}