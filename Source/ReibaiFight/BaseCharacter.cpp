// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

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

	//まず親クラスのTakeDamageを呼び出して、最終的なダメージ量を計算してもらう (重要！)
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);


	UE_LOG(LogTemp, Error, TEXT("[C++] %s took %f damage. Current Health: %f"), *GetName(), DamageApplied, CurrentHealth);


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
	UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetName());
}