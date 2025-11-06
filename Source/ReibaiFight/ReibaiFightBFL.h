// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"// データテーブルを使う場合に必要
#include "Containers/Set.h"
#include "BaseCharacter.h"
#include "ReibaiFightBFL.generated.h"

UCLASS()
class REIBAIFIGHT_API UReibaiFightBFL : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	// 指定した範囲内の敵にダメージを与える静的関数
	UFUNCTION(BlueprintCallable, Category = "Combat")
	static void ApplyRadialDamage(
		AActor* Attacker,//攻撃者
		FVector Origin,//攻撃の中心位置
		float Radius,//攻撃のサイズ（半径）
		float BaseDamage,//与えるダメージ
		TSet<AActor*>& HitActors
	);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	static ABaseCharacter* GetNearestEnemy(
		const UObject* WorldContextObject, 
		FVector Origin, 
		float Radius,
		AActor* ActorToIgnore //無視するアクタ（自分自身）
	);
};
