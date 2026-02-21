// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TeamID = EEnemyTeam::Ally;
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	//すでに死亡している場合は何もしない
	if (CurrentHealth <= 0.0f)
	{
		return 0;
	}

	//ダメージを与えたキャラクター
	ABaseCharacter* AttackerCharacter = nullptr;

	//EventInstigatorつまり攻撃を与えた者のコントローラーが存在する場合
	if (EventInstigator)
	{
		//TakeDmageを呼び出したキャラクターを取得
		AttackerCharacter = Cast<ABaseCharacter>(EventInstigator->GetPawn());
	}

	//陣営が同じときダメージ無効(フレンドリーファイアOff)
	if (AttackerCharacter && AttackerCharacter->TeamID == this->TeamID) {
		//自傷ダメージ、爆風でジャンプなど攻撃を利用する場合は必要
		//if(AttackerCharacter!=this)
		//{
			//同じ陣営ならダメージを受けない
			return 0.0f;
		//}
	}

	//まず親クラスのTakeDamageを呼び出して、最終的なダメージ量を計算してもらう (重要！)
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (CurrentHealth > 0.0f) {

		CurrentHealth -= DamageApplied;

		//OnHealthUpdated(CurrentHealth, MaxHealth);
	}
	return DamageApplied; // 実際に適用されたダメージ量を返す
}

void ABaseCharacter::Die()
{
	// 基本的な死亡処理はここに実装
	// 継承先のクラスで具体的な処理を実装することを想定
	//UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetName());
}