// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpirit.h"
#include "Kismet/GameplayStatics.h"//ApplyDamgeのために必要
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Kismet/KismetSystemLibrary.h" //For SphereOverlapActors
#include "ReibaiFightCharacter.h"       //Include player character header
#include "DrawDebugHelpers.h"           //For debug sphere
#include "ReibaiFightBFL.h"//自作ライブラリのインクルード

// Sets default values
AEnemySpirit::AEnemySpirit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    AttackEnemyDamage = 12.0f;
	CurrentHealth = MaxHealth;
    //カプセルコンポーネントがオーバーラップしたら'OnOverlapBegin'関数を呼び出す
    GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpirit::OnOverlapBegin);
}

// Called when the game starts or when spawned
void AEnemySpirit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemySpirit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemySpirit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AEnemySpirit::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    //UE_LOG(LogTemp, Error, TEXT("ENEMY TakeDamage called!"))

    float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    //ダメージを食らった時の処理
    if (DamageApplied > 0.f) {

		//体力が０以下になったら死亡処理を呼び出す
        if (CurrentHealth <= 0.0f)
        {
            //アクターをワールドから消滅させる
            // 死亡時のアニメーション再生
            Die();
        }
        //else {
        //    // ダメージを受けた際のリアクション（例: フラッシュエフェクトやサウンド）
        //    // ここにリアクションの処理を入れる
        //}
    }

    return DamageApplied;
}

void AEnemySpirit::Die()
{
    // 1. AIの思考を停止させる
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->GetBrainComponent()->StopLogic("Dead");
    }

    // 2. 当たり判定を消して、他のキャラクターの邪魔にならないようにする
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->DisableMovement();

	// 3. 経験値アイテムドロップの処理
    if (ExperienceParticleClass)
    {
        FVector SpawnLocation = GetActorLocation();
        FRotator SpawnRotation = GetActorRotation();

        GetWorld()->SpawnActor<AActor>(ExperienceParticleClass, SpawnLocation, SpawnRotation);
    }

    // 4. 死亡アニメーションを再生する
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && DeathMontage)
    {
        AnimInstance->Montage_Play(DeathMontage);
    }

    // 5. アニメーションの長さに合わせて、少し後に消滅させる
    SetLifeSpan(3.0f); // 3秒後にこのアクターを自動的に消滅させる

    //死亡時のアニメーションなくてもいいかも
    //このままだとスポーン、デストロイを繰り返し重くなる。
}

//void AEnemySpirit::Attack()
//{
//	//前回までの攻撃でヒットしたアクターのリストをクリア(多段ヒット防止用)
//    HitActorsDuringAttack.Empty();
//
//	//アニメーションインスタンスを取得
//    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
//
//    //アニメーションインスタンスと再生すべき攻撃モンタージュが存在するか確認
//    if (AnimInstance&&AttackMontage)
//    {
//		//攻撃モンタージュが再生中でない場合にのみ再生
//        //近いときにずっと攻撃モーションをすることを防ぐ
//        if (AnimInstance && !AnimInstance->Montage_IsPlaying(AttackMontage))
//        {
//            //UE_LOG(LogTemp, Log, TEXT("Enemy playing Attack Montage"));
//            AnimInstance->Montage_Play(AttackMontage);
//        }
//	}
//    else
//    {
//        //切り分け用ログ
//        if(!AnimInstance)
//        {
//            UE_LOG(LogTemp, Warning, TEXT("Enemy Attack(): AnimInstance is null"));
//		}
//        if (!AttackMontage)
//        {
//            UE_LOG(LogTemp, Warning, TEXT("Enemy Attack(): AttackMontage is not set in Blueprint"));
//        }
//	}
//}

void AEnemySpirit::AttackHitCheck(float DamageAmount)
{
    //UE_LOG(LogTemp, Log, TEXT("Enemy AttackHitCheck CALLED !"));
	
    // --- 敵自身の攻撃の中心位置と半径を計算 ---
	//GetActorLocation()で敵の中心位置、GetActorForwardVector()は単位ベクトルだから100.0fを掛け算して前方にずらす
    //const FVector HitOrigin = GetActorLocation() + GetActorForwardVector() * 100.0f; // 例: 自分の前方100ユニット

    //const float HitRadius = 80.0f; // 例: 敵の攻撃半径

    //UReibaiFightBFL::ApplyRadialDamage(
    //    this,
    //    HitOrigin,
    //    HitRadius,
    //    AttackEnemyDamage,
    //    HitActorsDuringAttack //多段ヒット防止の配列
    //);
    //DrawDebugSphere(GetWorld(), HitOrigin, HitRadius, 12, FColor::Orange, false, 2.0f);
}

void AEnemySpirit::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//プレイヤーをPlayerCharacterクラスとしてキャスト
    AReibaiFightCharacter* PlayerCharacter = Cast<AReibaiFightCharacter>(OtherActor);

    //このイベントが発生するとき、触れたものがプレイヤーでかつ自分自身でない時
    if (PlayerCharacter && OtherActor != this)
    {
        // プレイヤーなら、自分（敵）の攻撃力でダメージを与える
        UGameplayStatics::ApplyDamage(
            PlayerCharacter,
            AttackEnemyDamage, // 敵が持つ攻撃力変数
            GetController(),
            this,
            UDamageType::StaticClass()
        );

        // TODO: 多段ヒットを防ぐため、一度ダメージを与えたらタイマーで少しの間、再攻撃しないようにする
    }
}