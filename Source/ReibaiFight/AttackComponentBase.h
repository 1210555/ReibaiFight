#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyDataTypes.h"
#include "AttackComponentBase.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REIBAIFIGHT_API UAttackComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttackComponentBase();

protected:
	virtual void BeginPlay() override;

	//この攻撃の攻撃間隔（秒）。デフォルト値を設定。
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackRate = 5.0f;

	// このコンポーネント専用のタイマーハンドル
	FTimerHandle AttackTimerHandle;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// C++とブループリントの両方から呼び出せる攻撃実行関数
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void PerformAttack(); // C++の子クラスがoverrideするための 'virtual' 関数

	// このコンポーネントの自動攻撃タイマーを開始（または更新）する関数
	virtual void ActivateAttack();

	//子クラスでアップグレードを実装
	virtual void Upgrade(const FUpgradeData& UpgradeData);
};