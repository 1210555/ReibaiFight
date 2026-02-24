// Copyright Epic Games, Inc. All Rights Reserved.

#include "ReibaiFightCharacter.h"
#include "ReibaiFightBFL.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Kismet/GameplayStatics.h"//ApplyDamageに必要
#include "Kismet/KismetMathLibrary.h"
#include "Animation/AnimInstance.h"      // AnimInstanceを使う場合に必要
#include "DrawDebugHelpers.h"            // デバッグ表示に必要
#include "Blueprint/UserWidget.h" // UUserWidgetを使う場合に必要
#include "MyDataTypes.h"
#include "PIDTrackingComponent.h"
#include "HitodamaBase.h"



DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AReibaiFightCharacter

AReibaiFightCharacter::AReibaiFightCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AReibaiFightCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();


	//割り当てた行動を教える
	//IMC Defaultに追加する
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// DefaultMappingContext は .h ファイルで UPROPERTY として宣言しておく必要があります
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	CurrentHealth = MaxHealth;
	CurrentSpiritPower = MaxSpiritPower;

	//体力バーの初期化（最初はcurrentとmaxどちらも100）
	OnHealthUpdated(CurrentHealth, MaxHealth);
	OnExperienceUpdated(CurrentXP, XPToNextLevel);

	if (HitodamaClass)
	{
		// 例えば 10個 予備を作っておく
		int32 PoolSize = 3;

		for (int32 i = 0; i < PoolSize; i++)
		{
			// スポーンパラメータ設定
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = GetInstigator();

			// ひとだまを生成
			//スポーンしたひとだまのポインタをNewHitodamaに格納
			AHitodamaBase* NewHitodama = GetWorld()->SpawnActor<AHitodamaBase>(
				HitodamaClass,//ひとだまのBPクラスとして定義したもの（ヘッダファイルでTSubClassObjectで宣言している）
				GetActorLocation(),//ひとだまのスポーンした位置取得
				FRotator::ZeroRotator,//ひとだまのスポーンした角度取得
				SpawnParams//スポーンしたときのパラメータ
			);

			if (NewHitodama)
			{
				// 最初は非表示・停止状態にしておく
				NewHitodama->Deactivate();

				// 配列（プール）に追加
				HitodamaPool.Add(NewHitodama);
			}
		}
	}

}

//キャラクターがプレイヤーに操作され始める最初の一回だけ呼ばれる
//初期設定用の関数
void AReibaiFightCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	//if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	//{
	//	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	//	{
	//		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	//	}
	//}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
//BindActionでは
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AReibaiFightCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AReibaiFightCharacter::Look);

		//攻撃の動作
		EnhancedInputComponent->BindAction(StrongPunch1Action, ETriggerEvent::Triggered, this, &AReibaiFightCharacter::Attack);

		//Punch1Actionという名前の入力があったら&AReibaiFightCharacter::Punch1という関数を呼び出す
		EnhancedInputComponent->BindAction(Punch1Action, ETriggerEvent::Triggered, this, &AReibaiFightCharacter::Punch1);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AReibaiFightCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AReibaiFightCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AReibaiFightCharacter::Attack(const FInputActionValue& Value)
{
	//これがないと多段ヒット防止のためのHitActorsに一度しかはいらず
	//2回目以降の攻撃が当たらなくなる
	HitActors.Empty(); //新しい攻撃のたびにヒットしたアクターのリストをクリア

	bool bPressed = Value.Get<bool>();
	if (bPressed)
	{
		//攻撃アニメーションを再生する
		if (StrongPunch1Montage)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && !AnimInstance->Montage_IsPlaying(StrongPunch1Montage))
			{
				AnimInstance->Montage_Play(StrongPunch1Montage);
				AnimInstance->Montage_JumpToSection(FName("Attack1"), StrongPunch1Montage);
			}
		}
	}
}

void AReibaiFightCharacter::Punch1(const FInputActionValue& Value)
{
	HitActors.Empty(); //新しい攻撃のたびにヒットしたアクターのリストをクリア
	// ログを出して、関数が呼ばれたか確認
	//UE_LOG(LogTemp, Warning, TEXT("Punch1 function called!"));

	bool bPressed = Value.Get<bool>();
	if (bPressed)
	{
		// Punch1のアニメーションを再生する
		if (Punch1Montage)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance && !AnimInstance->IsAnyMontagePlaying())
			{
				AnimInstance->Montage_Play(Punch1Montage);
			}
		}
	}
}

void AReibaiFightCharacter::AttackHitCheck(float DamageAmount)
{

	//--- 当たり判定の中心位置を計算 ---
	//拳のソケット位置を取得 (ソケット名 "Fist_R_Socket")
	const FVector HitOrigin = GetMesh()->GetSocketLocation("Fist_R_Socket");
	const float HitRadius = 80.0f; // この攻撃の半径

	// --- BFLの関数を呼び出す ---
	// HitActors は .h で定義した TSet<AActor*> 型のメンバー変数
	UReibaiFightBFL::ApplyRadialDamage(
		this,        // 攻撃者は自分自身
		HitOrigin,   // 拳の位置
		HitRadius,   // 設定した半径
		DamageAmount,// この攻撃のダメージ量 (AnimNotifyから渡される)
		HitActors    // この攻撃でヒット済みのリスト
	);

	// デバッグ用に当たり判定の球を視覚化
	//DrawDebugSphere(GetWorld(), HitOrigin, HitRadius, 12, FColor::Red, false, 2.0f);
}

float AReibaiFightCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{

	//まず親クラスのTakeDamageを呼び出して、最終的なダメージ量を計算してもらう (重要！)
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	

	if (DamageApplied > 0.f) {

		OnHealthUpdated(CurrentHealth, MaxHealth);

		// 体力がゼロ以下になったかチェック
		if (CurrentHealth <= 0.0f)
		{
			//プレイヤーの死亡処理 (例: ゲームオーバー画面表示、リスタートなど)をいれる
			Die();
		}
		else
		{
			//プレイヤーの被ダメージリアクション (画面の点滅、サウンド再生など)をいれる
		}
	}
	return DamageApplied; // 実際に適用されたダメージ量を返す
}

void AReibaiFightCharacter::Die()
{

	APlayerController* PlayerController = GetController<APlayerController>();

	// UIが存在するかチェック
	if (GameOverWidgetClass && PlayerController)
	{
		// ゲームを一時停止し、マウスカーソルを表示
		PlayerController->SetPause(true);
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetInputMode(FInputModeUIOnly()); // マウス操作をUIのみに限定

		// UIを作成して画面に追加
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();
		}
	}

	// キャラクターの当たり判定と動きを停止
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->DisableMovement();
	DisableInput(PlayerController); // プレイヤーの入力を無効化
}

//void AReibaiFightCharacter::StartAutoAttackTimer()
//{
//	// 既にタイマーが動いている場合は何もしない
//	if (GetWorld()->GetTimerManager().IsTimerActive(AutoAttackTimerHandle))
//	{
//		return;
//	}
//
//	//UE_LOG(LogTemp, Warning, TEXT("Auto Attack Timer Started!"));
//	GetWorld()->GetTimerManager().SetTimer(
//		AutoAttackTimerHandle,
//		this,
//		&AReibaiFightCharacter::TriggerAutoAttack,
//		2.0f, // 2秒ごとに実行 (この間隔は後で調整可能)
//		true  // 繰り返し
//	);
//}

//タイマーで定期的に行われる処理
//void AReibaiFightCharacter::TriggerAutoAttack()
//{
//	//UE_LOG(LogTemp, Log, TEXT("Triggering All Auto Attacks..."));
//	// 所持している全ての自動攻撃コンポーネントの攻撃を実行
//	for (UAttackComponentBase* AttackComp : ActiveAttackComponents)
//	{
//		if (AttackComp)
//		{
//			AttackComp->PerformAttack();
//		}
//	}
//}

void AReibaiFightCharacter::GainExperience(int32 XPAmount)
{
	CurrentXP += XPAmount;

	// もし現在の経験値が必要経験値を超えたらレベルアップ
	if (CurrentXP >= XPToNextLevel)
	{
		LevelUp();
	}

	UE_LOG(LogTemp, Log, TEXT("Current XP: %d Next XP Amount %d !"), CurrentXP, XPToNextLevel);
	OnExperienceUpdated(CurrentXP, XPToNextLevel); //経験値バーの更新。中身はBP
}

void AReibaiFightCharacter::LevelUp()
{
	// レベルアップ処理
	CurrentLevel++;
	OnCurrentLevelUpdated(CurrentLevel); //HUDの現在のレベルの更新。中身はBP

	// 余った経験値を次のレベルに持ち越す
	CurrentXP -= XPToNextLevel;

	// 次のレベルに必要な経験値を増やす。1,2倍にする
	XPToNextLevel = FMath::RoundToInt(XPToNextLevel * 1.2f);

	//ここで次のレベルまでの経験値の上限を決めてもいいかも

	if (UpgradesDataTable) // データテーブルが設定されているかチェック
	{
		//以前のリストをクリア
		//これないとレベルアップ時UIにどんどん追加されてしまう
		OfferedUpgradeIDs.Empty();

		//データテーブルから全ての行の名前を取得
		TArray<FName> AllUpgradeIDs = UpgradesDataTable->GetRowNames();

		//抽選対象の「候補」を入れるための、空のリストを新しく用意
		TArray<FName> AvailableUpgradeIDs;

		//データテーブルの全てのアップグレードを1つずつチェック
		for (const FName& UpgradeID : AllUpgradeIDs)
		{
			// データテーブルからアップグレードの情報を取得
			FUpgradeData* Row = UpgradesDataTable->FindRow<FUpgradeData>(UpgradeID, TEXT(""));
			if (!Row) continue; // データが見つからなければスキップ

			//    「前提条件が設定されていない(None)」または「前提条件IDを既に取得済み」
			bool bPrerequisiteMet = Row->PrerequisiteID.IsNone() || AcquiredUpgradeIDs.Contains(Row->PrerequisiteID);
			if (!bPrerequisiteMet)
			{
				// 前提条件を満たしていないものは除外
				continue;
			}

			//「既に取得済み」かつ「繰り返し取得不可」
			bool bAlreadyAcquired = AcquiredUpgradeIDs.Contains(UpgradeID);
			// 3. 両方の条件を満たした場合のみ、抽選「候補」リストに追加
			if (bAlreadyAcquired && !Row->bIsRepeatable)
			{
				continue;
			}
			AvailableUpgradeIDs.Add(UpgradeID);
		}

		// リストをシャッフル
		// 配列をシャッフルしてランダムな順番にする
		const int32 Num = AvailableUpgradeIDs.Num();
		for (int32 i = 0; i < Num; ++i)
		{
			int32 j = FMath::RandRange(i, Num - 1);
			if (i != j)
			{
				AvailableUpgradeIDs.Swap(i, j);
			}
		}

		// シャッフルしたリストの先頭から3つをOfferedUpgradeIDsに追加
		int32 NumToOffer = FMath::Min(3, AvailableUpgradeIDs.Num());
		for (int32 i = 0; i < NumToOffer; ++i)
		{
			OfferedUpgradeIDs.Add(AvailableUpgradeIDs[i]);
		}
	}

	// レベルアップ報酬の選択UIを表示し、ゲームを一時停止する処理を呼び出す
	OnLevelUp();
}

//Upgradeはレベルアップ時に選択したアップグレードの名前
void AReibaiFightCharacter::ApplyUpgradeByID(FName UpgradeID)
{
	//取得済みリストに追加
	//これをもとに強化などを行う
	AcquiredUpgradeIDs.Add(UpgradeID);

	//データテーブルから、選択されたアップグレードの完全なデータを取得
	FUpgradeData* UpgradeData = UpgradesDataTable->FindRow<FUpgradeData>(UpgradeID, TEXT(""));
	if (!UpgradeData)
	{
		//データが見つからなかった場合のエラー処理
		UE_LOG(LogTemp, Error, TEXT("Could not find UpgradeData for ID: %s"), *UpgradeID.ToString());
		return;
	}

	//取得したUpgradeData構造体のTypeに応じて処理を分岐
	switch (UpgradeData->Type)
	{
	//攻撃追加
	case EUpgradeType::AddNewAttack:
		//以下でAddComponentByClass を使ったコンポーネント追加処理
		//TagがHitodamaの攻撃追加
		if (UpgradeData->AttackComponentTag == FName("Hitodama"))
		{
			//プールにまだ予備があるかを確認
			if (HitodamaPool.IsValidIndex(ActiveHitodamaCount))
			{
				//プールから次の人魂を取り出して「起動」
				HitodamaPool[ActiveHitodamaCount]->Activate(GetRootComponent());

				//初期の配置角度を0度に設定
				HitodamaPool[ActiveHitodamaCount]->SetOrbitAngle(0.0f); 

				//ここでカウントを増やす
				//１番目(ActiveHitodamaCountの初期値は１)はもう使ったから、次は２番目を使うという意味
				ActiveHitodamaCount++;

			}
		}
		//設定されているかいないかのみのtrue,false
		//UpgradeDataのAttackComponentClassがあるかどうか
		else if(UpgradeData->AttackComponentClass)
		{
			//指定されたクラスのコンポーネントを自分に追加
			//型がUActorComponentのため以下でキャストが必要
			UActorComponent* AddedComponent = AddComponentByClass(UpgradeData->AttackComponentClass, false, FTransform(), false);

			//追加されたコンポーネントをAttackComponentBase型にキャスト
			UAttackComponentBase* NewAttack = Cast<UAttackComponentBase>(AddedComponent);

			if (NewAttack)
			{
				NewAttack->RegisterComponent(); // コンポーネントを登録
				NewAttack->ActivateAttack();
				OnUpgradeAcquired(*UpgradeData);
			}
		}
		break;

	//体力回復
	case EUpgradeType::ModifyPlayerStat:
	{
		// TMap<FName, float> StatModifications の中身を一つずつ取り出す
		for (const auto& Elem : UpgradeData->StatModifications)
		{
			//データテーブルのStatModifications項目の左がキーである何を変更したいか
			//右がどれくらい変更するか
			// キー (例: "MaxHealth", "Heal")、値 (例: 20.0, 50.0)
			const FName StatName = Elem.Key;   
			const float Value = Elem.Value;

			// キーの名前に応じて処理を分岐
			if (StatName == "MaxHealth")
			{
				MaxHealth += Value;
				CurrentHealth += Value; // 最大値が上がった分、現在値も回復
				OnHealthUpdated(CurrentHealth, MaxHealth); // UIに通知
			}
			else if (StatName == "Heal")
			{
				CurrentHealth = FMath::Min(CurrentHealth + Value, MaxHealth);
				OnHealthUpdated(CurrentHealth, MaxHealth); // UIに通知
			}
			else if (StatName == "FullHeal")
			{
				CurrentHealth = MaxHealth;
				OnHealthUpdated(CurrentHealth, MaxHealth); // UIに通知
			}
			else if (StatName == "MovementSpeed")
			{
				GetCharacterMovement()->MaxWalkSpeed += Value;
			}
			else if (StatName == "AllyAttackChance")
			{
				AllyChance += Value;
			}
		}
		break;
	}

	//既存の能力強化
	case EUpgradeType::ModifyAttackStat:

		//強化対象のコンポーネントを探す
		TArray<UActorComponent*> Components;
		GetComponents(Components); // 自分についている全コンポーネントを取得

		for (UActorComponent* Component : Components)
		{
			//指定されたタグを持つコンポーネントか？
			if (Component->ComponentHasTag(UpgradeData->AttackComponentTag))
			{
				//専門家（攻撃コンポーネント）に仕事を任せる
				UAttackComponentBase* AttackComp = Cast<UAttackComponentBase>(Component);
				if (AttackComp)
				{
					//AttackComponentBaseクラスのUpdate関数にUpgradeData構造体を引数とする
					AttackComp->Upgrade(*UpgradeData);
					OnUpgradeAcquired(*UpgradeData);
				}
			}
		}

		//ひとだまを強化
		//親クラスがActorのため同じところでアップデートができないため
		//ここで強化を行う。
		if (UpgradeData->AttackComponentTag == FName("Hitodama") || UpgradeData->StatModifications.Contains("Quantity"))
		{
			//既存のすべてのひとだまを強化（ダメージUPなど）
			//Quantityの場合はここでは強化は行わない
			for (TObjectPtr<AHitodamaBase> Hitodama : HitodamaPool)
			{
				if (Hitodama)
				{
					Hitodama->Upgrade(*UpgradeData);
				}
			}

			//数を増やす処理 (ModifyPlayerStatからここに移動)
			if (UpgradeData->StatModifications.Contains("Quantity"))
			{
				int32 AddCount = FMath::RoundToInt(UpgradeData->StatModifications["Quantity"]);
				UE_LOG(LogTemp, Warning, TEXT("Hitodama Quantity Increasing by: %d"), AddCount);

				int32 OldCount = ActiveHitodamaCount;
				int32 NewTotalCount = OldCount + AddCount;

				// プールの上限を超えないようにガード（念のため）
				if (NewTotalCount > HitodamaPool.Num())
				{
					NewTotalCount = HitodamaPool.Num();
				}

				//増やしたいひとだまをまず起動する
				for(int32 i= OldCount; i< NewTotalCount; ++i)
				{
					if (HitodamaPool.IsValidIndex(i))
					{
						// 次の人魂を起動
						HitodamaPool[i]->Activate(GetRootComponent());
						// 出した瞬間にも強化を適用（Lv1の強さで出ないように）
						HitodamaPool[i]->Upgrade(*UpgradeData);
					}
				}

				//カウントを正式に更新
				ActiveHitodamaCount = NewTotalCount;
				if (ActiveHitodamaCount > 0) {
					float AngleStep = 360.0f / ActiveHitodamaCount;
					for (int32 i = 0; i < ActiveHitodamaCount; ++i)
					{
						if (HitodamaPool.IsValidIndex(ActiveHitodamaCount))
						{
							float NewAngle = i * AngleStep;

							//角度だけを変えたいから
							HitodamaPool[i]->SetOrbitAngle(NewAngle);
						}
					}
				}
			}
		}
		break;
	}

	//---ゲームに戻る処理---
	
	//ゲームの一時停止を解除
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		PlayerController->SetPause(false);
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly()); // 入力モードをゲームのみに戻す
	}

	//レベルアップUIを画面から削除
	if (LevelUpWidgetInstance)
	{
		LevelUpWidgetInstance->RemoveFromParent();
		LevelUpWidgetInstance = nullptr; // 変数をクリア
	}
}

//void AReibaiFightCharacter::ActivateAllHitodamas()
//{
//	// ここでループを回す（これが部長の仕事）
//
//	int32 Count = 3; // 出したい数（変数にしてもOK）
//	float AngleStep = 360.0f / Count; // 120度ずつ
//
//	for (int32 i = 0; i < Count; i++)
//	{
//		// プールからひとだまを取り出す
//		if (HitodamaPool.IsValidIndex(i))
//		{
//			float NewAngle = i * AngleStep; // 0, 120, 240...
//
//			// ここで、さきほど改造した「角度指定ありのActivate」を呼ぶ
//			HitodamaPool[i]->Activate(GetRootComponent(), NewAngle);
//		}
//	}
//}
//
//// 死亡時などに呼ぶ関数
//void AReibaiFightCharacter::DeactivateAllHitodamas()
//{
//	// 持っている数が0なら、ループに入らないので何も起きない（安全）
//	for (int32 i = 0; i < ActiveHitodamaCount; i++)
//	{
//		if (HitodamaPool.IsValidIndex(i))
//		{
//			HitodamaPool[i]->SetActorHiddenInGame(true);
//			HitodamaPool[i]->SetActorTickEnabled(false);
//		}
//	}
//}