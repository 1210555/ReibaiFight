// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ReibaiFightCharacter.h" //親クラスのインクルード
#include "EnemySpirit.generated.h"

UCLASS()
class REIBAIFIGHT_API AEnemySpirit : public AReibaiFightCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemySpirit();



protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//死亡時のアニメーション用のモンタージュ
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;

	void Die();

	// 死亡時にドロップする経験値アイテムのブループリント
	UPROPERTY(EditAnywhere, Category = "Loot")
	TSubclassOf<AActor> ExperienceParticleClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;

	TSet<AActor*> HitActorsDuringAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackEnemyDamage;//敵の攻撃力(コンストラクタで初期化)

	UFUNCTION(BlueprintCallable, Category = "Combat",meta=(ToolTip="Enemy Attack animaiton and Logic!!!!!!!!"))
	void Attack();

	virtual void AttackHitCheck(float DamageAmount) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//ダメージ処理
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
