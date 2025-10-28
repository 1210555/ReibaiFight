#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackComponentBase.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REIBAIFIGHT_API UAttackComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttackComponentBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// C++とブループリントの両方から呼び出せる攻撃実行関数
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void PerformAttack(); // C++の子クラスがoverrideするための 'virtual' 関数

	// ブループリント側でイベントとして実装するための関数 (今回は使いませんが、こういう書き方もあります)
	// UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	// void PerformAttack_Blueprint();

	//子クラスでアップグレードを実装
	virtual void Upgrade(float ModificationValue);
};