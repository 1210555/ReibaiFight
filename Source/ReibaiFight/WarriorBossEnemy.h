// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "WarriorBossEnemy.generated.h"

/**
 * 
 */

class UNiagaraSystem;

UCLASS()
class REIBAIFIGHT_API AWarriorBossEnemy : public ABaseEnemy
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	UAnimMontage* AttackGroundMontage;

	UPROPERTY(EditDefaultsOnly, Category="Combat")
	UAnimMontage* AttackByWeaponMontage;

	//hÇ≈êÈåæÇ∑ÇÈÇ∆Ç´ÇÕTObjectPtrÇégÇ§ÇÃÇ™êÑèßÇ≥ÇÍÇƒÇ¢ÇÈ
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TObjectPtr<UNiagaraSystem> ShockWaveNiagaraSystem;

protected:

	void Die() override;

	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	void FullHealing();

	void BeginPlay() override;

	void Tick(float DeltaTime) override;


public:
	//AWarriorBossEnemy();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttackGround();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void AttackByWeapon();

	UFUNCTION(BlueprintCallable, Category= "Combat")
	void SpawnShockWaveEffect();

};
