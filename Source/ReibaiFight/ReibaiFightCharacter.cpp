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
#include "EnemySpirit.h"
#include "Animation/AnimInstance.h"      // AnimInstanceを使う場合に必要
#include "DrawDebugHelpers.h"            // デバッグ表示に必要
#include "Blueprint/UserWidget.h" // UUserWidgetを使う場合に必要
#include "MyDataTypes.h"



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
	CurrentHealth = MaxHealth;
	CurrentSpiritPower = MaxSpiritPower;

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
}

//////////////////////////////////////////////////////////////////////////
// Input
//キャラクターがプレイヤーに操作され始める最初の一回だけ呼ばれる
//初期設定用の関数
void AReibaiFightCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
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
		UE_LOG(LogTemp, Warning, TEXT("Attack Action. SetupPlayerInoutComponent"));
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
	//UE_LOG(LogTemp, Warning, TEXT("Move"));
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
	// input is a bool
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
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
	UE_LOG(LogTemp, Warning, TEXT("Punch1 function called!"));

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
	UE_LOG(LogTemp, Warning, TEXT("Player AttackHitCheck CALLED via BFL!"));

	// --- 当たり判定の中心位置を計算 ---
	// 拳のソケット位置を取得 (ソケット名 "Fist_R_Socket" は要確認)
	const FVector HitOrigin = GetMesh()->GetSocketLocation("Fist_R_Socket");
	const float HitRadius = 80.0f; // この攻撃の半径

	// --- BFLの関数を呼び出してダメージ適用 ---
	// HitActors は .h で定義した TSet<AActor*> 型のメンバー変数
	UReibaiFightBFL::ApplyRadialDamage(
		this,        // 攻撃者は自分自身
		HitOrigin,   // 拳の位置
		HitRadius,   // 設定した半径
		DamageAmount,// この攻撃のダメージ量 (AnimNotifyから渡される)
		HitActors    // この攻撃でヒット済みのリスト
	);

	// デバッグ用に当たり判定の球を視覚化
	DrawDebugSphere(GetWorld(), HitOrigin, HitRadius, 12, FColor::Red, false, 2.0f);
}

float AReibaiFightCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Player TakeDamage Started! DamageAmount: %f"), DamageAmount);
	// まず親クラスのTakeDamageを呼び出して、最終的なダメージ量を計算してもらう (重要！)
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// 体力が残っていればダメージを適用
	if (CurrentHealth > 0.0f)
	{
		CurrentHealth -= DamageApplied;
		UE_LOG(LogTemp, Warning, TEXT("Player took %f damage. Current Health: %f"), DamageApplied, CurrentHealth);

		if (HUDWidgetInstance) // HUDウィジェットへの参照が存在するかチェック
		{
			// 1. 呼び出すイベント名を指定
			FName FuncName = FName("UpdateHealthBar");

			// 2. ウィジェットからその名前の関数(イベント)を探す
			UFunction* Function = HUDWidgetInstance->FindFunction(FuncName);

			// 3. 関数が見つかったら
			if (Function)
			{
				// 4. イベントに渡す引数を準備するための構造体
				struct { float Current; float Max; } Params;
				Params.Current = CurrentHealth; // 現在の体力をセット
				Params.Max = MaxHealth;       // 最大体力もセット

				// 5. イベントを実行し、引数を渡す
				HUDWidgetInstance->ProcessEvent(Function, &Params);
			}
		}

		// 体力がゼロ以下になったかチェック
		if (CurrentHealth <= 0.0f)
		{
			// TODO: プレイヤーの死亡処理 (例: ゲームオーバー画面表示、リスタートなど)
			UE_LOG(LogTemp, Error, TEXT("PLAYER DIED!"));
			// DisableInput(GetController<APlayerController>()); // 操作不能にするなど
		}
		else
		{
			// TODO: プレイヤーの被ダメージリアクション (画面の点滅、サウンド再生など)
		}
	}

	return DamageApplied; // 実際に適用されたダメージ量を返す
}

//ブルプリから呼び出す（自動攻撃開始の合図）
void AReibaiFightCharacter::EnableAutoAttack()
{
	// すでに自動攻撃が有効な場合は何もしない
	if (bHasAutoAttack) return;

	bHasAutoAttack = true;

	// 自分に追加されている攻撃コンポーネントを探し出す
	CurrentAttackComponent = FindComponentByClass<UAttackComponentBase>();

	if (CurrentAttackComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Auto Attack Enabled!"));

		// タイマーを開始する
		GetWorld()->GetTimerManager().SetTimer(
			AutoAttackTimerHandle,
			this,
			&AReibaiFightCharacter::TriggerAutoAttack,
			2.0f, // 2秒ごとに実行
			true  // 繰り返し
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnableAutoAttack failed: No Attack Component found!"));
	}
}

//タイマーで定期的に行われる処理
void AReibaiFightCharacter::TriggerAutoAttack()
{
	if (CurrentAttackComponent)
	{
		// 装備している攻撃コンポーネントに攻撃を実行させる
		CurrentAttackComponent->PerformAttack();
	}
}

void AReibaiFightCharacter::GainExperience(int32 XPAmount)
{
	CurrentXP += XPAmount;
	UE_LOG(LogTemp, Warning, TEXT("Gained %d XP. Current XP: %d / %d"), XPAmount, CurrentXP, XPToNextLevel);

	// もし現在の経験値が必要経験値を超えたらレベルアップ
	if (CurrentXP >= XPToNextLevel)
	{
		LevelUp();
	}
}

void AReibaiFightCharacter::LevelUp()
{
	// レベルアップ処理
	CurrentLevel++;

	// 余った経験値を次のレベルに持ち越す
	CurrentXP -= XPToNextLevel;

	// 次のレベルに必要な経験値を増やす（例：1.5倍にする）
	XPToNextLevel = FMath::RoundToInt(XPToNextLevel * 1.5f);

	UE_LOG(LogTemp, Warning, TEXT("LEVEL UP! New Level: %d. Next XP: %d"), CurrentLevel, XPToNextLevel);


	if (UpgradesDataTable) // データテーブルが設定されているかチェック
	{
		//以前のリストをクリア
		//これないとレベルアップ時UIにどんどん追加されてしまう
		OfferedUpgradeIDs.Empty();

		//データテーブルから全ての行の名前を取得
		TArray<FName> AllUpgradeIDs = UpgradesDataTable->GetRowNames();

		// TODO: ここに「取得済みではない」「前提条件を満たしている」などのフィルタリング処理を追加する

		// リストをシャッフル
		// 配列をシャッフルしてランダムな順番にする
		const int32 Num = AllUpgradeIDs.Num();
		for (int32 i = 0; i < Num; ++i)
		{
			int32 j = FMath::RandRange(i, Num - 1);
			if (i != j)
			{
				AllUpgradeIDs.Swap(i, j);
			}
		}

		// シャッフルしたリストの先頭から3つをOfferedUpgradeIDsに追加
		int32 NumToOffer = FMath::Min(3, AllUpgradeIDs.Num());
		for (int32 i = 0; i < NumToOffer; ++i)
		{
			OfferedUpgradeIDs.Add(AllUpgradeIDs[i]);
		}
	}

	// レベルアップ報酬の選択UIを表示し、ゲームを一時停止する処理を呼び出す
	OnLevelUp();
}

void AReibaiFightCharacter::ApplyUpgrade(FName UpgradeID)
{
	/*UE_LOG(LogTemp, Warning, TEXT("Upgrade Selected: %s"), *UpgradeID.ToString());

	// TODO: UpgradeIDに応じて、実際に能力を適用する処理をここに書く
	// (例: 新しい攻撃コンポーネントを追加する、ステータスを上げるなど)

	// --- ゲームに戻る処理 ---

	// 1. ゲームの一時停止を解除
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		PlayerController->SetPause(false);
		PlayerController->bShowMouseCursor = false;
		// ...
	}

	// 2. レベルアップUIを画面から削除
	if (LevelUpWidgetInstance)
	{
		LevelUpWidgetInstance->RemoveFromParent();
	}*/
}

//Indexはレベルアップ時に選択したアップグレードの配列内インデックス(レベルアップ時UIの左の項目から０，１，２が入る)
void AReibaiFightCharacter::ApplyUpgradeByID(FName UpgradeID)
{
	// OfferedUpgrades配列が有効なインデックスかチェック
	//OfferedUpgradesはデータテーブルの行の名前の配列
	// 
	//if (!OfferedUpgradeIDs.IsValidIndex(Index))//Index=0のとき一番左のボタンに書かれたアップグレード項目の名前が入る
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Invalid Upgrade Index: %d"), Index);
	//	return;
	//}

	//選択されたアップグレードの行の名前を取得
	//const FName UpgradeID = OfferedUpgradeIDs[Index];
	
	UE_LOG(LogTemp, Warning, TEXT("Upgrade Selected: %s"), *UpgradeID.ToString());

	//取得済みリストに追加
	AcquiredUpgradeIDs.Add(UpgradeID);

	//データテーブルから、選択されたアップグレードの完全なデータを取得
	FUpgradeData* UpgradeData = UpgradesDataTable->FindRow<FUpgradeData>(UpgradeID, TEXT(""));
	if (!UpgradeData)
	{
		//データが見つからなかった場合のエラー処理
		UE_LOG(LogTemp, Error, TEXT("Could not find UpgradeData for ID: %s"), *UpgradeID.ToString());
		// ... ゲームを再開する処理 ...
		return;
	}

	//取得したUpgradeData構造体のTypeに応じて処理を分岐
	switch (UpgradeData->Type)
	{
	//攻撃追加
	case EUpgradeType::AddNewAttack:
		// ... (AddComponentByClass を使ったコンポーネント追加処理) ...
		if(UpgradeData->AttackComponentClass)	//AttackComponentClassに設計図が指定されているか？
		{
			//指定されたクラスのコンポーネントを自分に追加
			//型がUActorComponentのため以下でキャストが必要
			UActorComponent* AddedComponent = AddComponentByClass(UpgradeData->AttackComponentClass, false, FTransform(), false);

			//追加されたコンポーネントをAttackComponentBase型にキャスト
			UAttackComponentBase* NewAttack = Cast<UAttackComponentBase>(AddedComponent);

			if (NewAttack)
			{
				NewAttack->RegisterComponent(); // コンポーネントを登録
				UE_LOG(LogTemp, Warning, TEXT("Added new attack component: %s"), *NewAttack->GetName());
			}
		}
		break;
	//体力回復
	case EUpgradeType::ModifyPlayerStat:
		if (UpgradeData->PlayerStatToModify == "MaxHealth")
		{
			MaxHealth += UpgradeData->ModificationValue;
			CurrentHealth = MaxHealth;
		}
		break;
	//既存の能力強化
	case EUpgradeType::ModifyAttackStat:
		// 強化対象のコンポーネントを探す
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
					AttackComp->Upgrade(UpgradeData->ModificationValue);
				}
				break;
			}
		}
		break;
	}


	// --- ゲームに戻る処理 ---

	// 1. ゲームの一時停止を解除
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		PlayerController->SetPause(false);
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly()); // 入力モードをゲームのみに戻す
	}

	// 2. レベルアップUIを画面から削除
	if (LevelUpWidgetInstance)
	{
		LevelUpWidgetInstance->RemoveFromParent();
		LevelUpWidgetInstance = nullptr; // 変数をクリア
	}
}