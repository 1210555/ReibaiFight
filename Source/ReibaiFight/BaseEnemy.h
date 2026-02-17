// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "GameTypes.h"
#include "Components/WidgetComponent.h"
#include "BaseEnemy.generated.h"

/**
 * 
 */
//UENUM(BlueprintType)
//enum class EEnemyTeam :uint8
//{
//	Enemy   UMETA(DisplayName = "Enemy"),
//	Ally    UMETA(DisplayName = "Ally")
//};

 //AEXperiecneの前方宣言。インクルードせずにExperienceクラスを使用できる
class AExperiecne;
class UBehaviorTree;

UCLASS()
class REIBAIFIGHT_API ABaseEnemy : public ABaseCharacter
{
	GENERATED_BODY()

private:

protected:
	virtual void BeginPlay() override;

	// 死亡時にドロップする経験値アイテムのブループリント
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AExperience> ExperienceParticleClass;

	//敵の攻撃モーションだけどいらないかも

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void AttackHitCheck(float DamageAmount);

	//UIの作業のためBPで中身の実装を行う
	//UI,きっかけは基本BPで行うが、これはきっかけをc++で行う
	//敵の周囲に与えたダメージ量を表示する
	UFUNCTION(BlueprintImplementableEvent, Category = "Combat")
	void AppearDamageAmount(float DamageAmount);

	//敵のカプセルコンポーネントにオーバーラップしたときの関数
	UFUNCTION()
	void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float OverlapEnemyDamage;//敵の攻撃力(コンストラクタで初期化)

	//死亡時のアニメーション用のモンタージュ
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;

	//死亡時に再生するNiagaraエフェクト
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TObjectPtr<class UNiagaraSystem> DeathNiagaraSystem;

	// この敵が落とす経験値の合計量
	// EditAnywhere にすることで、BP側で変更できるように
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float DropExperienceAmount = 100.0f;

	// 経験値アイテムをスポーンさせる関数
	void SpawnExperience();

	UFUNCTION(BlueprintCallable,Category="Spawn")
	void Activate(FVector SpawnLocation);

	UFUNCTION(BlueprintCallable, Category = "Spawn")
	void Deactivate();

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Stats")
	bool bIsDead = false;

	//UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	//EEnemyTeam EnemyTeam = EEnemyTeam::Enemy;

	UFUNCTION(BlueprintCallable,Category="Stats")
	void ConvertToAlly();

	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnAllyHealthUpdated(float Current, float Max);
	
	//味方時の体力バーUIコンポーネント
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	UWidgetComponent* AllyHealthWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* AllyBehaviorTree;

public:
	ABaseEnemy();

	virtual void Tick(float DeltaTime) override;

	//死亡時のアニメーションは敵ごとに分ける
	//基本ここでセットしたアニメーションにするが、
	//継承先でオーバーライドして変えることもできるようにしておく
	virtual void Die() override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//敵がどのビヘイビアツリーを使うかを設定するための変数
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* EnemyBehaviorTree;
};
