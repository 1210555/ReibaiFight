// Fill out your copyright notice in the Description page of Project Settings.


#include "WarriorBossEnemy.h"
#include "NiagaraFunctionLibrary.h"//Niagaraの関数ライブラリを使うために必要
#include "NiagaraComponent.h"//NiagaraComponentを使うために必要
#include "ReibaiFightBFL.h"//自作ライブラリ、UGameStatics::SpawnEmitterAtLocation関数を使うために必要
#include "Kismet/GameplayStatics.h"//GameStaticsのために必要


void AWarriorBossEnemy::BeginPlay()
{
	Super::BeginPlay();
	CurrentHealth = MaxHealth;

}

void AWarriorBossEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWarriorBossEnemy::Die()
{
	Super::Die();
	UE_LOG(LogTemp, Warning, TEXT("WarriorBossEnemy Die() called!"));

}

float AWarriorBossEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	//体力が半分以下になったら全回復
	if (CurrentHealth < MaxHealth * 0.5f)
	{
		FullHealing();
	}
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void AWarriorBossEnemy::AttackGround()
{
	//アニメーションインスタンスを取得
	UAnimInstance* AnimInstance_Ground = GetMesh()->GetAnimInstance();
	//アニメーションインスタンス、再生すべき攻撃モンタージュが存在するか確認
	if (AnimInstance_Ground && AttackGroundMontage)
	{
		//モンタージュが再生中か
		if (!AnimInstance_Ground->Montage_IsPlaying(AttackGroundMontage))
		{
			AnimInstance_Ground->Montage_Play(AttackGroundMontage);
		}
	}
}

void AWarriorBossEnemy::AttackByWeapon()
{
	UAnimInstance* AnimInstance_Weapon = GetMesh()->GetAnimInstance();

	if (AnimInstance_Weapon && AttackByWeaponMontage) 
	{
		if(!AnimInstance_Weapon->Montage_IsPlaying(AttackByWeaponMontage))
		{
			AnimInstance_Weapon->Montage_Play(AttackByWeaponMontage);
		}
	}
}

void AWarriorBossEnemy::FullHealing()
{
	UE_LOG(LogTemp, Warning, TEXT("FullHealing called (WarriorBossEnemy)"))
	MaxHealth = CurrentHealth * 1.5;
	CurrentHealth = MaxHealth;
}

void AWarriorBossEnemy::SpawnShockWaveEffect()
{
	/*UGameplayStatics::SpawnEmitterAtLocation(
		this,
		ShockWaveNiagaraSystem,
		GetActorLocation(),
		FRotator::ZeroRotator,
		FVector(1.0f),
		true
	);*/
}