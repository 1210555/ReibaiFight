// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ReibaiFightCharacter.h" //�e�N���X�̃C���N���[�h
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

	//���S���̃A�j���[�V�����p�̃����^�[�W��
	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* DeathMontage;

	void Die();

	// ���S���Ƀh���b�v����o���l�A�C�e���̃u���[�v�����g
	UPROPERTY(EditAnywhere, Category = "Loot")
	TSubclassOf<AActor> ExperienceParticleClass;

	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;

	TSet<AActor*> HitActorsDuringAttack;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float AttackEnemyDamage;//�G�̍U����(�R���X�g���N�^�ŏ�����)

	UFUNCTION(BlueprintCallable, Category = "Combat",meta=(ToolTip="Enemy Attack animaiton and Logic!!!!!!!!"))
	void Attack();

	virtual void AttackHitCheck(float DamageAmount) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//�_���[�W����
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
