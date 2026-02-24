// Fill out your copyright notice in the Description page of Project Settings.
//このクラスは追従機能を持たせるために使うクラス。
// ProjectileMovementComponentのホーミング機能、当たり判定のスフィアコリジョンを利用して、ターゲットに向かって飛ぶ弾の基本的な動きを実装する。

#include "AttackProjectileBase.h"
#include "BaseCharacter.h"
#include "Components/SphereComponent.h"//CollisionCompのために必要
#include "NiagaraComponent.h"//NiagaraCompのために必要
#include "GameFramework/ProjectileMovementComponent.h"//ProjectileMovementのために必要
#include "ReibaiFightBFL.h" // ApplyRadialDamage関数を呼び出すために必要

// Sets default values
AAttackProjectileBase::AAttackProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    //当たり判定の作成
    CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
    CollisionComp->InitSphereRadius(15.0f);//DamageRadiusでいいかも？
    CollisionComp->SetCollisionProfileName(TEXT("OverlapAllDynamic")); //  何にでも重なる設定
    RootComponent = CollisionComp; // これをアクタの中心（ルート）にする

    //ナイアガラを作成し、くっつける処理
    NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
    NiagaraComp->SetupAttachment(RootComponent);

    //追尾機能の作成と設定
    ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
    ProjectileMovement->UpdatedComponent = CollisionComp;
    ProjectileMovement->InitialSpeed = 1000.f;
    ProjectileMovement->MaxSpeed = 1000.f;
    ProjectileMovement->ProjectileGravityScale = 0.0f; // 重力ゼロ（まっすぐ飛ぶ）

    // ホーミング（追尾）の基本設定をONにしておく
    ProjectileMovement->bIsHomingProjectile = true;
    ProjectileMovement->HomingAccelerationMagnitude = 2000.0f; // 曲がる力
}

// Called when the game starts or when spawned
void AAttackProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	
    // 自分が何かに重なった時（Overlap）、OnSphereOverlap関数を呼ぶように「予約」する
    if (CollisionComp)
    {
        CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AAttackProjectileBase::OnSphereOverlap);
    }

    // 万が一どこにも当たらず飛んでいっても、5秒後に自動で消滅させる（メモリのゴミ箱行きを防ぐ）
    SetLifeSpan(5.0f);
}

// Called every frame
void AAttackProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// 敵や壁に当たった時の処理
void AAttackProjectileBase::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 自分自身や、撃った人（ShooterCharacter）には当たらないようにする
    if (OtherActor && OtherActor != this && OtherActor != ShooterCharacter)
    {
        ABaseCharacter* HitCharacter = Cast<ABaseCharacter>(OtherActor);
        if (!HitCharacter) return;

        //当たったら消えるもののためこのスコープで多重判定のチェック配列を作成
        TSet<AActor*> HitActors;
		UE_LOG(LogTemp, Warning, TEXT("Projectile hit: %s"), *OtherActor->GetName());
        UReibaiFightBFL::ApplyRadialDamage(
            ShooterCharacter,                 // 攻撃者
            GetActorLocation(),               // ダメージの中心点（弾の位置）
            DamageRadius,                          // ダメージ範囲の半径
            DamageAmount,                           // ダメージ量（仮）
            HitActors                  // この攻撃でヒットしたアクターのリスト（今回は空でOK）
		);
        // 当たったら自分（弾）は消滅する
        Destroy();
    }
}