// Fill out your copyright notice in the Description page of Project Settings.


#include "ReibaiFightBFL.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemySpirit.h" // 敵クラスのヘッダー

//ダメージを与える関数、引数は
//(攻撃者、攻撃の中心位置、攻撃のサイズ（半径）、与えるダメージ)
void UReibaiFightBFL::ApplyRadialDamage(AActor* Attacker, FVector Origin, float Radius, float BaseDamage, TSet<AActor*>& HitActors)
{

	UE_LOG(LogTemp, Warning, TEXT("BFL ApplyRadialDamage Started! Attacker: %s"), *Attacker->GetName());

	if (!Attacker) return;

	//攻撃者がプレイヤーか敵かを判定
	bool bAttackerIsPlayer = Attacker->IsA(AReibaiFightCharacter::StaticClass());

	//ダメージを与える対象のクラスを決定
	UClass* ClassToTarget = bAttackerIsPlayer ? AEnemySpirit::StaticClass() : AReibaiFightCharacter::StaticClass();

	//無視するアクターのリスト（攻撃者自身）
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Attacker);

	//検出するアクターの配列
	TArray<AActor*> OverlappingActors;

	//指定した位置で球状のオーバーラップ判定を実行
	bool bIsHit = UKismetSystemLibrary::SphereOverlapActors(
		Attacker->GetWorld(),
		Origin,
		Radius,
		{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) },
		ClassToTarget,
		ActorsToIgnore,
		OverlappingActors
	);

	if (bIsHit)
	{
		for (AActor* HitActor : OverlappingActors)
		{
			if (!HitActors.Contains(HitActor))
			{
				//誰にダメージ与えたか
				UE_LOG(LogTemp, Warning, TEXT("BFL Applying Damage to: %s"), *HitActor->GetName());

				// ヒットした敵にダメージを与える
				UGameplayStatics::ApplyDamage(
					HitActor,
					BaseDamage,
					Attacker->GetInstigatorController(), // 攻撃者のコントローラーを取得
					Attacker,
					UDamageType::StaticClass()
				);
				HitActors.Add(HitActor); //多段ヒット防止のためのヒットリストに追加
			}
		}
	}
}

AEnemySpirit* UReibaiFightBFL::GetNearestEnemy(const UObject* WorldContextObject, FVector Origin, float Radius)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AEnemySpirit::StaticClass(), FoundActors);

	AActor* NearestActor = nullptr;
	float MinDistance = Radius;

	for (AActor* Actor : FoundActors)
	{
		float Distance = FVector::Dist(Origin, Actor->GetActorLocation());
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			NearestActor = Actor;
		}
	}

	return Cast<AEnemySpirit>(NearestActor);
}