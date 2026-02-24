// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AttackProjectileBase.generated.h"

//class AActor;
class ABaseCharacter;
class UNiagaraComponent;
class UProjectileMovementComponent;
class USphereComponent;


UCLASS()
class REIBAIFIGHT_API AAttackProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAttackProjectileBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//以下publicの３つはAutoShadowBallAttackなど追従系の攻撃クラスでセットしてもらうための変数
	//誰が撃ったか（ダメージ計算などに使う）
	UPROPERTY(BlueprintReadWrite, Category = "Attack")
	ABaseCharacter* ShooterCharacter;

	//誰を追いかけるか（ホーミング用）
	UPROPERTY(BlueprintReadWrite, Category = "Attack")
	class AActor* TargetActor;

	// 弾の移動を管理（追従機能）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attack")
	float DamageAmount = 30.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float DamageRadius = 30.0f;

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComp;

	// ナイアガラエフェクト
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UNiagaraComponent* NiagaraComp;

	UFUNCTION()
	void OnSphereOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);



	//TSet<AActor*> HitActors; //多段ヒット防止のためのヒットリスト
};

// Fill out your copyright notice in the Description page of Project Settings.

//#pragma once
//
//#include "CoreMinimal.h"
//#include "GameFramework/Actor.h"
//#include "ShadowBallBase.generated.h"
//
//class UNiagaraComponent;
//class UProjectileMovementComponent;
//class USphereComponent;
//
//UCLASS()
//class REIBAIFIGHT_API AShadowBallBase : public AActor
//{
//	GENERATED_BODY()
//
//public:
//	// Sets default values for this actor's properties
//	AShadowBallBase();
//
//protected:
//	// Called when the game starts or when spawned
//	virtual void BeginPlay() override;
//
//public:
//	AShadowBallBase();
//
//	// 当たり判定（これがルートになります）
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
//	USphereComponent* CollisionComp;
//
//	// ナイアガラエフェクト
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
//	UNiagaraComponent* NiagaraComp;
//
//	// 弾の移動を管理する超便利コンポーネント
//	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
//	UProjectileMovementComponent* ProjectileMovement;
//
//protected:
//
//	UFUNCTION(BlueprintCallable, Category = "ShadowBall")
//	void SpawnShadowBallEffect();
//
//};
