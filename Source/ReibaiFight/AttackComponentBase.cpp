#include "AttackComponentBase.h"

UAttackComponentBase::UAttackComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttackComponentBase::BeginPlay()
{
	Super::BeginPlay();
}

void UAttackComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// PerformAttackの本体。中身は空で、子クラスによって上書きされることを想定。
void UAttackComponentBase::PerformAttack()
{
	// This function is intended to be overridden by child classes
}

//このクラスを継承したクラス（つまり攻撃系の子クラス）で内容を実装
void UAttackComponentBase::Upgrade(const FUpgradeData& UpgradeData)
{
	// 基本クラスでは何もしない。子クラスがこの中身を実装する。
}

//自動攻撃のタイマーを開始、更新する関数
void UAttackComponentBase::ActivateAttack()
{
	// 既存のタイマーをクリア
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);

	// 新しい攻撃間隔(AttackRate)で、PerformAttack関数を繰り返し呼び出すタイマーを開始
	GetWorld()->GetTimerManager().SetTimer(
		AttackTimerHandle,
		this,
		&UAttackComponentBase::PerformAttack,
		AttackRate, // このコンポーネントの攻撃間隔
		true,       // 繰り返し
		0.0f        // 最初の攻撃はすぐに実行
	);
}