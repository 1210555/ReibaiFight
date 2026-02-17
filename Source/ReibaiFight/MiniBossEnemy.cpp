// Fill out your copyright notice in the Description page of Project Settings.

#include "MiniBossEnemy.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"


AMiniBossEnemy::AMiniBossEnemy()
{
	OverlapEnemyDamage = 23.0f;
}

void AMiniBossEnemy::BeginPlay()
{
    Super::BeginPlay();
	CurrentHealth = MaxHealth;

    //よくないやり方らしいが、いったんこのまま。
	// ToDo：c++でコンポーネントを作成してそれをBPで編集する
	//以下BPで作成したコンポーネントをc++で紐づける方法
	TArray<UBoxComponent*> BoxComponents;
	GetComponents<UBoxComponent>(BoxComponents);

	//BoxComponents配列の中身をBoxCompに入れて試していく。今回はWeaponCollisionBoxと一致するものがあればループを出る
    for(UBoxComponent* BoxComp : BoxComponents)
    {
        if(BoxComp && BoxComp->GetName() == TEXT("WeaponCollisionsBox"))
        {
            WeaponCollisionBox = BoxComp;
            break;
        }
	}
}

void AMiniBossEnemy::Attack_Normal()
{
	//前回までの攻撃でヒットしたアクターのリストをクリア(多段ヒット防止用)
    HitActorsDuringAttack.Empty();

	//アニメーションインスタンスを取得
    UAnimInstance* AnimInstance_Nor = GetMesh()->GetAnimInstance();

    //アニメーションインスタンスと再生すべき攻撃モンタージュが存在するか確認
    if (AnimInstance_Nor && NormalAttackMontage)
    {
		//攻撃モンタージュが再生中でない場合にのみ再生
        //近いときにずっと攻撃モーションをすることを防ぐ
        if (AnimInstance_Nor && !AnimInstance_Nor->Montage_IsPlaying(NormalAttackMontage))
        {
            AnimInstance_Nor->Montage_Play(NormalAttackMontage);
        }
	}
}

void AMiniBossEnemy::Attack_Angry()
{
    //前回までの攻撃でヒットしたアクターのリストをクリア(多段ヒット防止用)
    HitActorsDuringAttack.Empty();

    //アニメーションインスタンスを取得
    UAnimInstance* AnimInstance_Ang = GetMesh()->GetAnimInstance();

    //アニメーションインスタンスと再生すべき攻撃モンタージュが存在するか確認
    if (AnimInstance_Ang && AngryAttackMontage)
    {
        //攻撃モンタージュが再生中でない場合にのみ再生
        //近いときにずっと攻撃モーションをすることを防ぐ
        if (AnimInstance_Ang && !AnimInstance_Ang->Montage_IsPlaying(AngryAttackMontage))
        {
            AnimInstance_Ang->Montage_Play(AngryAttackMontage);
        }
    }
}

void AMiniBossEnemy::Die()
{
    Super::Die();
    UE_LOG(LogTemp, Warning, TEXT("MiniBossEnemy Die() called!"));
    SetLifeSpan(1.5f);
}

float AMiniBossEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    UE_LOG(LogTemp, Warning, TEXT("TakeDamage: HP=%f / %f"), CurrentHealth, MaxHealth);
    if (CurrentHealth <= MaxHealth * 0.5f)
    {
        UE_LOG(LogTemp, Warning, TEXT("HP is under 50%%! Trying to get Controller..."));
        AAIController* AICon = Cast<AAIController>(GetController());

		// AIContorollerが存在しかつ、Blackboardコンポーネントが存在するか確認
        if (AICon && AICon->GetBlackboardComponent())
        {
            // 怒りモード(1)へ移行
			//列挙型のキー'BattleState'をBBで作成、その一つ目(0)がNormal、二つ目(1)がAngry
            AICon->GetBlackboardComponent()->SetValueAsEnum(TEXT("BattleState"), 1);
            UE_LOG(LogTemp, Warning, TEXT("MiniBossEnemy has entered Angry State! (Success)"));
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("AIController is NULL! (Maybe Unpossessed?)"));
		}
    }

    return ActualDamage;
}

void AMiniBossEnemy::EnableWeaponCollision()
{
    if(WeaponCollisionBox == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponCollisionsBox is NULL!"));
        return;
	}
	//武器の当たり判定をオンにする。これをANで呼び出す
    WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//多重ヒット防止用にヒットしたアクターのリストをクリア
	HitActorsDuringAttack.Empty();
}

void AMiniBossEnemy::DisableWeaponCollision()
{
    if(WeaponCollisionBox == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("WeaponCollisionBox is NULL!"));
        return;
	}
	//武器の当たり判定をオフにする。これをANで呼び出す
    WeaponCollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}