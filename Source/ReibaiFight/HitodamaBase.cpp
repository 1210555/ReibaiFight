// Fill out your copyright notice in the Description page of Project Settings.
//ひとだま攻撃のプーリングの土台となるクラス

#include "HitodamaBase.h"
#include "Components/SphereComponent.h"//SphereComponentを使うために必要
#include "DrawDebugHelpers.h"//デバッグ用、ひとだまの攻撃を与える判定を視覚化


// Sets default values
AHitodamaBase::AHitodamaBase()
{
	// Tick（毎フレーム更新）は、普段は切っておき、起動時に有効にする
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false); // デフォルトはオフ

	// PIDコンポーネントを作成し、ルートに設定
	PIDTrackingComponent = CreateDefaultSubobject<UPIDTrackingComponent>(TEXT("PIDController"));
	RootComponent = PIDTrackingComponent;

	// Niagaraコンポーネントを作成し、PIDコンポーネントにアタッチ
	NiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("Effect"));
	NiagaraComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AHitodamaBase::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AHitodamaBase::Tick(float DeltaTime)
{
	//Super::Tick(DeltaTime);

}

// 「起動」された時の処理
void AHitodamaBase::Activate(USceneComponent* TargetToFollow)
{
	////ひとだまのTickを有効にする
	//SetActorTickEnabled(true);
	//UE_LOG(LogTemp, Error, TEXT("Hitodama Activate Function CALLED!"));

	//// 衝突無視の設定
	//// TargetToFollow（追従先）があるなら、その持ち主（プレイヤー）とは当たらないようにする
	//if (TargetToFollow)
	//{
	//	AActor* OwnerActor = TargetToFollow->GetOwner(); // コンポーネントから持ち主のアクターを取得
	//	if (OwnerActor)
	//	{
	//		// SphereCollisionが有効なら、持ち主アクターを無視リストに追加
	//		//これがないとひとだま起動時にプレイヤーに当たりクラッシュの原因となる
	//		if (SphereCollision)
	//		{
	//			SphereCollision->IgnoreActorWhenMoving(OwnerActor, true);
	//		}
	//		// 念のため、Instigatorもセットしておく
	//		SetInstigator(Cast<APawn>(OwnerActor));
	//	}
	//}
	////ひとだまを表示させる
	////SetActorHiddenInGameをtrue,flaseにすることでひとだまをだしたり、隠したりする
	////これがオブジェクトプーリング設計の核
	//SetActorHiddenInGame(false);
	//NiagaraComponent->Activate(); // エフェクトを再生
	//// PIDコンポーネントに「追いかける相手」を教える
	//if (PIDTrackingComponent)
	//{
	//	PIDTrackingComponent->ResetState(); // 内部状態をリセット
	//	PIDTrackingComponent->SetTargetToFollow(TargetToFollow);
	//	PIDTrackingComponent->Activate(); // PIDコンポーネントのTickも有効化
	//	PIDTrackingComponent->SetComponentTickEnabled(true);
	//	
	//}
	////アクター自身のTickも有効に戻す（コンストラクタで切ったので）
	//SetActorTickEnabled(true);

}

// 「停止（凍結）」された時の処理
void AHitodamaBase::Deactivate()
{
	//ひとだま消滅時はこれをtrueにして非表示にする
	//存在が消えているわけではない。メモリ上には存在しており、再度Activateするときもヒープ領域を確保する必要はない
	SetActorHiddenInGame(true);   
	NiagaraComponent->Deactivate(); // エフェクトを停止

	if (PIDTrackingComponent)
	{
		PIDTrackingComponent->SetTargetToFollow(nullptr); // 追従ターゲットを解除
		//PIDTrackingComponent->Deactivate(); // PIDコンポーネントのTickも停止
	}
}

void AHitodamaBase::Upgrade(const FUpgradeData& UpgradeData)
{

	//エフェクトの差し替え
	if (UpgradeData.NewNiagaraEffect && NiagaraComponent)
	{
		NiagaraComponent->SetAsset(UpgradeData.NewNiagaraEffect);
	}

	if (UpgradeData.StatModifications.Contains("Damage"))
	{
		// 存在したら、その値を使ってダメージを強化
		BaseDamage += UpgradeData.StatModifications["Damage"];
	}

	//UE_LOG(LogTemp, Warning, TEXT("Hitodama Upgraded!"));
}

bool AHitodamaBase::HasMatchingTag(FName TagToCheck) const
{
	return Tags.Contains(TagToCheck);
}

void AHitodamaBase::SetOrbitAngle(float NewAngle)
{
	if (PIDTrackingComponent)
	{
		// PIDコンポーネントの関数を呼び出す
		PIDTrackingComponent->SetInitialAngle(NewAngle);
	}
}

void AHitodamaBase::PerformHitodamaAttack() {
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	AActor* Hitodama = this;//ここでひとだま自身を指す?

	//今回の攻撃でヒットした敵を記録するための、空のリストを作る
	TSet<AActor*> HitActorsInThisAttack;

	//DrawDebugSphere(
	//	GetWorld(),
	//	GetActorLocation(),
	//	AttackRadius,
	//	12,
	//	FColor::Red,
	//	false,
	//	3.0f
	//);
	// 当たり判定のロジックは、ライブラリの関数を呼び出すだけでいい
		UReibaiFightBFL::ApplyRadialDamage(
		OwnerCharacter,                 // 攻撃者
		Hitodama->GetActorLocation(),// ダメージの中心点（敵の位置）
		AttackRadius,                         // ダメージ範囲の半径
		BaseDamage,                     // ダメージ量
		HitActorsInThisAttack           // この攻撃でヒットしたアクターのリスト
	);
		//だれにどれだけダメージを与えたかログに出す
		for (AActor* HitActor : HitActorsInThisAttack)
		{
			UE_LOG(LogTemp, Warning, TEXT("Hitodama Attack hit: %s for %f damage"), *HitActor->GetName(), BaseDamage);
		}
}