// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum class EEnemyTeam :uint8
{
	Enemy   UMETA(DisplayName = "Enemy"),
	Ally    UMETA(DisplayName = "Ally")
};

UCLASS()
class REIBAIFIGHT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 1000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	//死亡時に呼び出される関数
	//継承するクラスで実装する
	virtual void Die();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//ダメージを受け取る関数のオーバーライド
	//これはACharacterクラスで定義されている関数だからオーバーライドで中身を実装
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	float GetCurrentHealth() const { return CurrentHealth; }

	//全キャラクターに所属するチームの札を持たせる(Enemy,Ally)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	EEnemyTeam TeamID;

};