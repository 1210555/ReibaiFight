// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseEnemy.h"
#include "AIController.h"//AAIControllerクラスを使うために必要
#include "BehaviorTree/BlackboardComponent.h" //BlackboardComponentを使うために必要
#include "BrainComponent.h"//AIContollerのGetBrainComponentを使うために必要
#include "Components/CapsuleComponent.h"//CapsuleComponentを使うために必要
#include "EnemyHealthWidget.h"//敵の体力UIウィジェットクラスを使うために必要
#include "GameFramework/CharacterMovementComponent.h"//CharacterMovementを使うために必要
#include "Kismet/GameplayStatics.h"//GameStaticsのために必要
#include "ReibaiFightCharacter.h"//AReibaiFightCharacterクラスを使うために必要
#include "ReibaiFightBFL.h"//自作ライブラリ、ApplyRadialDamage関数を使うために必要
#include "ReibaiFightGameInstance.h"//ゲームインスタンスを使うために必要
#include "Experience.h"//AExperienceクラスを使うために必要
#include "NiagaraFunctionLibrary.h"//Niagaraの関数ライブラリを使うために必要
#include "NiagaraComponent.h"//NiagaraComponentを使うために必要

ABaseEnemy::ABaseEnemy()
{
    OverlapEnemyDamage = 5.0f;
	//PrimaryActorTick.bCanEverTick = true;
	TeamID = EEnemyTeam::Enemy;

	// UWidgetComponent型のAllyHealthWidgetという名前のパーツを作成。以下,AllyHealthWidgetCompという変数名でアクセス可能 
    AllyHealthWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("AllyHealthWidget"));

    AllyHealthWidgetComp->SetupAttachment(RootComponent);
    //  常にカメラの方を向くようにする
    // これがビルボード！
    AllyHealthWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);

    // 位置調整：頭の上に（Z軸を+100くらい）
    AllyHealthWidgetComp->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f));

    // サイズ調整
    AllyHealthWidgetComp->SetDrawSize(FVector2D(100.0f, 20.0f));

    // 初期状態は「非表示」（最初は敵なのでHPバーは見せない）
    AllyHealthWidgetComp->SetVisibility(false);
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();

	//敵の体力を最大体力で初期化
	//コンストラクタに書くと、BPで編集した値が反映されないため、BeginPlayで初期化する
    CurrentHealth = MaxHealth;

    if (CapsuleComp)
    {
		//カプセルコンポーネントがオーバーラップしたら'OnOverlapBegin'関数を呼び出すという登録をする
		CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnOverlapBegin);
	}

	MyGameInstance = Cast<UReibaiFightGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseEnemy::Die() {
    
    if (TeamID == EEnemyTeam::Enemy) {
		ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
		AReibaiFightCharacter* ReibaiFightCharacter = Cast<AReibaiFightCharacter>(PlayerCharacter);

        //キャスト成功かチェック
        if (ReibaiFightCharacter) {
			//敵を仲間にするチャンスがあるかチェック
            if (ReibaiFightCharacter->AllyChance > 0) {
                ConvertToAlly();
                ReibaiFightCharacter->AllyChance--;
                
				return; //仲間になったときは以降の死亡処理を行わない
            }
        }
    }

    AAIController* AIController = Cast<AAIController>(GetController());
    UBrainComponent* BrainComp = AIController->GetBrainComponent();
    if (AIController&&BrainComp)
    {
        if (BrainComp) {
            // AIの思考を停止させる
            BrainComp->StopLogic("Dead");
        }
    }

    // 当たり判定を消して、他のキャラクターの邪魔にならないようにする
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);//当たり判定を無効化
	GetCharacterMovement()->DisableMovement();//移動を無効化

    // 経験値アイテムドロップの処理
    if (ExperienceParticleClass)
    {
		SpawnExperience();
    }

	//死亡アニメーションを再生
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && DeathMontage)
    {
        AnimInstance->Montage_Play(DeathMontage);
    }

	FVector SpawnNiagaraLocation = GetActorLocation();
    if (DeathNiagaraSystem) // 事前にUPROPERTYで持たせておく
    {
        UNiagaraFunctionLibrary::SpawnSystemAtLocation(
            this,
            DeathNiagaraSystem,
            SpawnNiagaraLocation,
            FRotator::ZeroRotator,
            FVector(1.0f), // スケール
            true,          // Auto Destroy（再生終わったら消えるか）
            true,          // Auto Activate
            ENCPoolMethod::None,
            true           // PreCull Check
        );
    }

	float FinalManjuDropChance = BaseManjuDropChance;
    if (MyGameInstance)
    {
			FinalManjuDropChance *= MyGameInstance->DifficultyMultiplier; // ゲームインスタンスから難易度倍率を取得してドロップ率に反映
    }

    if (FMath::RandRange(0.0f, 1.0f) < FinalManjuDropChance) {
            //エディタでセットしたまんじゅうクラスあるかを確認
            UE_LOG(LogTemp, Warning, TEXT("Spawn　CHANCE！！！"));

        if (ManjuClass) {
            FVector SpawnLocation = GetActorLocation();
            FRotator SpawnRotation = FRotator::ZeroRotator;
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
            UE_LOG(LogTemp, Warning, TEXT("Spawn　Manju!!!!!!!!!!!!!!!!!!!"));
            GetWorld()->SpawnActor<AActor>(ManjuClass, SpawnLocation, SpawnRotation, SpawnParams);
        }
    }

    //見えなくして、当たり判定も消す。デストロイアクタするわけではない
    FTimerHandle DeactivateTimer;//Fがついてるから構造体。同じく構造体であるFVectorなどと同じように記述できる
    GetWorldTimerManager().SetTimer(DeactivateTimer, this, &ABaseEnemy::DeactivateEnemy, 2.0f, false);
}

void ABaseEnemy::ConvertToAlly() {
    TeamID = EEnemyTeam::Ally;
    CurrentHealth = MaxHealth;
	AllyHealthWidgetComp->SetVisibility(true);
    UE_LOG(LogTemp, Warning, TEXT("ConvertToAllyが呼ばれました!!!!!!!!!!!!!!!!!!!"));

	AAIController* AllyAI = Cast<AAIController>(GetController());
    if (AllyAI && AllyBehaviorTree)
    {
		float SerarchRadius = 5000.0f;
        ABaseCharacter* PlayerCharacter = UReibaiFightBFL::GetNearestEnemy(
            this,
			GetActorLocation(),
			SerarchRadius,
			this,
            this
        );
        
        if (PlayerCharacter) {
			//仲間用のビヘイビアツリーに切り替え
			AllyAI->RunBehaviorTree(AllyBehaviorTree);

			//ブラックボードのLeaderActorキーにプレイヤーキャラクターをセット
            AllyAI->GetBlackboardComponent()->SetValueAsObject(
                TEXT("LeaderActor"),
                PlayerCharacter
			);
        }
    }
}

void ABaseEnemy::SpawnExperience() {
	//敵の持ってる経験値
	int32 RemainingExperience = FMath::FloorToInt(DropExperienceAmount);

	int32 BigExperienceCount = RemainingExperience / 100;//100経験値アイテムの数
	RemainingExperience %= 100;

	int32 MiddleExperienceCount = RemainingExperience / 25;//25経験値アイテムの数
    RemainingExperience %= 25;

	int32 SmallExperienceCount = RemainingExperience / 5;//5経験値アイテムの数
    RemainingExperience %= 5;

	// ラムダ式でスポーン処理を共通化
    auto SpawnLoop = [&](int32 Count, int32 Value, float Scale)
        {
            if (!ExperienceParticleClass || Count <= 0) return;

            for (int32 i = 0; i < Count; i++)
            {
                FVector SpawnLocation = GetActorLocation();
                FRotator SpawnRotation = GetActorRotation();//経験値をスポーンさせる回転(敵の回転角、あんま関係ない)

                //経験値の位置を少しばらつかせる
                float SpawnRadius = 100.0f;
                SpawnLocation.X += FMath::RandRange(-SpawnRadius, SpawnRadius);
                SpawnLocation.Y += FMath::RandRange(-SpawnRadius, SpawnRadius);
                SpawnLocation.Z += 50.0f; // 少し浮かせる

                // スポーン実行
                AExperience* Particle =  GetWorld()->SpawnActor<AExperience>(ExperienceParticleClass, SpawnLocation, SpawnRotation);

                // スポーン成功したら、中身を設定する
                if (Particle)
                {
					// 経験値の量と大きさを初期化
                    Particle->InitializeExperience(Value, Scale);
                }
            }
        };
    // 第2引数 = 点数, 第3引数 = 大きさ倍率
    SpawnLoop(BigExperienceCount, 100, 4.0f); // 大: 100点, 2倍
    SpawnLoop(MiddleExperienceCount, 25, 2.0f); // 中:  25点, 1.2倍
    SpawnLoop(SmallExperienceCount, 5, 1.0f); // 小:   5点, 0.8倍
}

void ABaseEnemy::AttackHitCheck(float DamageAmout) {
    ////ダメージを与える中心点の位置を取得
    //FVector AttackOrigin = GetActorLocation() + GetActorForwardVector() * 100.0f; //キャラクターの前方100単位の位置
    ////ダメージを与える範囲の半径
    //float AttackRadius = 200.0f;
    ////ダメージを与える
    //UReibaiFightBFL::ApplyRadialDamage(this, DamageAmout, AttackOrigin, AttackRadius);
}

float ABaseEnemy::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) {
	//親クラス(BaseCharacter)のTakeDamageを呼び出して、実際にダメージを計算してもらう
    //TakeDamageの戻り値は食らったダメージ量
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    UE_LOG(LogTemp, Warning, TEXT("BaseEnemyのTakeDamageが呼ばれました"));

    //体力が0以下になったら死亡時のアニメーション
    if (CurrentHealth <= 0.0f) {
        Die();
    }

    if(ActualDamage > 0.f) {
		//BPで中身を実装している関数を呼び出す
        AppearDamageAmount(ActualDamage);//UIにダメージ量を表示する
	}

	//もし敵が仲間になっていたら、体力の更新をUIに伝える
    if (TeamID == EEnemyTeam::Ally) {
        // 0除算防止
        UE_LOG(LogTemp, Warning, TEXT("BaseEnemyのTakeDamageのAllyのとき！！！！！！！！！！！"));

        if (MaxHealth != 0.f) {
            UE_LOG(LogTemp, Warning, TEXT("BaseEnemyのTakeDamageのAlly not 0 MaxHealth"));

            float Percent = CurrentHealth / MaxHealth;

            //体力バーなかったら以下スキップ
            if (AllyHealthWidgetComp) {
                UE_LOG(LogTemp, Warning, TEXT("BaseEnemyのTakeDamageのAlly have Ally Health Widget"));

                UUserWidget* UserWidget = AllyHealthWidgetComp->GetUserWidgetObject();
                UEnemyHealthWidget* EnemyHealthWidget = Cast<UEnemyHealthWidget>(UserWidget);
                if (EnemyHealthWidget) {
                    UE_LOG(LogTemp, Warning, TEXT("Ally Update EnemyHealth"));

                    EnemyHealthWidget->UpdateHealthBar(Percent);
                }
            }
        }
    }
	return ActualDamage;
}

void ABaseEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    // 自分自身や、ダメージクールダウン中は無視
    if (OtherActor == this ) return;

    // 相手がプレイヤー（BaseCharacter）か確認
    ABaseCharacter* TargetCharacter = Cast<ABaseCharacter>(OtherActor);

	//HP増減などをもつBaseCharacterクラスであり、かつReibaiFightCharacterクラスであるか確認
    if (TargetCharacter && TargetCharacter->TeamID != this->TeamID)
    {
        // ダメージを与える
        UGameplayStatics::ApplyDamage(
            TargetCharacter,
            OverlapEnemyDamage,
            GetController(),
            this,
            UDamageType::StaticClass()
        );
    }
}

void ABaseEnemy::ActivateEnemy(FVector SpawnLocation, FRotator SpawnRotation) {

    if (USkeletalMeshComponent* MeshComp = GetMesh())
    {
        if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
        {
            // 再生中のモンタージュ（死亡アニメーション等）を0秒で強制停止する
            AnimInstance->StopAllMontages(0.0f);
        }
    }
    //敵に戻るときに必要。陣営をEnemyに戻し、体力バーも非表示
    TeamID = EEnemyTeam::Enemy;
    AllyHealthWidgetComp->SetVisibility(false);

    CurrentHealth = MaxHealth;
    bIsDead = false;

	SetActorLocationAndRotation(SpawnLocation, SpawnRotation);
    SetActorHiddenInGame(false);
    SetActorEnableCollision(true);
    SetActorTickEnabled(true);
    //体力を最大に回復

    
    if(GetCharacterMovement()) {
		GetCharacterMovement()->Velocity = FVector::ZeroVector; // 速度をリセット
        GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	}

    if (AAIController* AIController = Cast<AAIController>(GetController()))
    {
        if (UBrainComponent* BrainComp = AIController->GetBrainComponent())
        {
            //StopLogic("Dead") で止めた思考を、再度最初から実行させる
            BrainComp->RestartLogic();
        }
    }
}

void ABaseEnemy::DeactivateEnemy() {
    SetActorHiddenInGame(true);//隠す
    SetActorEnableCollision(false);//コリジョンを消す
    SetActorTickEnabled(false);//Tickを消す
    bIsDead = true;//死亡させる

    //移動速度を0にしておく。これしとかないとActivateEnemyしたときに移動速度が残ったままになる。
    if (GetCharacterMovement())
    {
        GetCharacterMovement()->StopMovementImmediately();
    }
}

bool ABaseEnemy::IsDead() const {
    return bIsDead;
}