// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyPoolManager.h"
#include "BaseEnemy.h"

// Sets default values
AEnemyPoolManager::AEnemyPoolManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//Managaer自体にTickは必要ないので、falseにしておく。
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AEnemyPoolManager::BeginPlay()
{
	Super::BeginPlay();
	
	//BPでクラスが設定されているか確認する。
	if (!EnemyClass) {
		UE_LOG(LogTemp, Warning, TEXT("EnemyClass is not set in EnemyPoolManager!"));
		return;
	}

	//指定された数だけ敵をスポーンして、プールに追加する。
	for(int32 i=0; i < PoolSize; i++)
	{
		//とりあえず
		ABaseEnemy* NewEnemy = GetWorld()->SpawnActor<ABaseEnemy>(EnemyClass, FVector::ZeroVector, FRotator::ZeroRotator);
		
		if (NewEnemy) {
			// ★後で ABaseEnemy に実装する「おやすみモード」にする関数を呼ぶ
			/*NewEnemy->DeactivateEnemy();*/

			// 配列に追加
			EnemyPool.Add(NewEnemy);
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("Failed to spawn enemy in EnemyPoolManager!"));
		}
	}
}

ABaseEnemy* AEnemyPoolManager::SpawnEnemyFromPool(FVector SpawnLocation, FRotator SpawnRotation)
{
	for(ABaseEnemy* Enemy : EnemyPool)
	{
		// 敵が存在していて、かつ死んでいる（＝プールに戻っている）場合
		if(Enemy && Enemy->IsDead())
		{
			Enemy->ActivateEnemy(SpawnLocation, SpawnRotation);
			// ★後で ABaseEnemy に実装する「おやすみモード」から復帰させる関数を呼ぶ
			/*Enemy->ActivateEnemy();*/
			return Enemy;
		}
	}

	return nullptr;
}
