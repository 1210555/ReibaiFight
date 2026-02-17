// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEnemy.h"
#include "MiniBossEnemy.generated.h"

/**
 * 
 */

class UBoxComponent;

UCLASS()
class REIBAIFIGHT_API AMiniBossEnemy : public ABaseEnemy
{
	GENERATED_BODY()
	
private:

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* NormalAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* AngryAttackMontage;

protected:
	void Die() override;

	void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UBoxComponent* WeaponCollisionBox;

	UPROPERTY()
	TArray<AActor*> HitActorsDuringAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float SwordDamage = 30.0f;

public:
	AMiniBossEnemy();

	UFUNCTION(BlueprintCallable, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	void Attack_Normal();

	UFUNCTION(BlueprintCallable, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	void Attack_Angry();

	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void EnableWeaponCollision();

	UFUNCTION(BlueprintCallable)
	void DisableWeaponCollision();

	/*UFUNCTION()
	void OnWeaponOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);*/
	
};
