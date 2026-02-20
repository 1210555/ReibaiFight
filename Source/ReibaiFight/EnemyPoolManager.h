// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyPoolManager.generated.h"

class ABaseEnemy;

UCLASS()
class REIBAIFIGHT_API AEnemyPoolManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyPoolManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	//プールに確保する敵の種類。これをBPで設定する。
	UPROPERTY(EditAnywhere, Category = "Enemy Pool")
	TSubclassOf<ABaseEnemy> EnemyClass;

	//最初から確保しておく敵の数。
	UPROPERTY(Editanywhere, Category="Enemy Pool")
	int32 PoolSize = 50;

	//プールに確保した敵を格納する配列。
	UPROPERTY()
	TArray<ABaseEnemy*> EnemyPool;

	UFUNCTION(BlueprintCallable, Category = "Enemy Pool")
	ABaseEnemy* SpawnEnemyFromPool(FVector SpawnLocation, FRotator SpawnRotation);
};
