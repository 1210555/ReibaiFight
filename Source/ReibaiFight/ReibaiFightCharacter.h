// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AttackComponentBase.h"
#include "MyDataTypes.h"
#include "BaseCharacter.h" 
#include "ReibaiFightCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AReibaiFightCharacter : public ABaseCharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	//攻撃の入力うけつけ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StrongPunch1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Punch1Action;

public:
	AReibaiFightCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxSpiritPower = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentSpiritPower;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//体力が更新された時にブループリント側で呼び出されるイベント
	//これによりイベント駆動型への移行
	UFUNCTION(BlueprintImplementableEvent, Category = "Health")
	void OnHealthUpdated(float Current, float Max); // ← これを追加

protected:
	//ただのvoid関数はUEエディタ内で使用できない、内部の計算などを行うときはただのvoidにする。

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Attack(const FInputActionValue& Value);

	void Punch1(const FInputActionValue& Value);

	//攻撃のアニメーション
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* StrongPunch1Montage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* Punch1Montage;

	//攻撃の通知が来た時にブループリント上で呼び出す
	UFUNCTION(BlueprintCallable, Category = "Combat",meta= (ToolTip = "for player")) // Anim Notifyから呼び出せるようにする
	virtual void AttackHitCheck(float DamageAmount);

	// 1回の攻撃で既にヒットしたアクターを格納する配列(多段ヒット防止)
	TSet<AActor*> HitActors;

	// 自動攻撃が有効かどうかのフラグ
	//これがtrueのときだけ自動攻撃を行う
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasAutoAttack = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TArray<TObjectPtr<UAttackComponentBase>> ActiveAttackComponents;

	// 自動攻撃のタイマーハンドル
	FTimerHandle AutoAttackTimerHandle;

	// EnableAutoAttack() の代わりに、タイマーを開始する専用の関数を用意
	void StartAutoAttackTimer();

	/** タイマーによって定期的に呼び出される関数 */
	void TriggerAutoAttack();

	//ゲームオーバーに表示するUIのブループリントクラス
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	//プレイヤーの死亡処理(ゲームオーバー時のUI表示)
	virtual void Die() override;

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Experience")
	int32 CurrentLevel = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Experience")
	int32 CurrentXP = 0;

	//次のレベルまでに必要な経験値
	// 1→2 のときは 100 , 2→3 のときは 100 * 1.5 , 3→4 のときは100 * 1.5 * 1.5 …
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Experience")
	int32 XPToNextLevel = 100;

	/* 経験値を獲得する関数、レベルアップ時にBPから呼ばれる*/
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void GainExperience(int32 XPAmount);

	// プレイヤーが既に取得したアップグレードのIDを格納する配列
	//自動斬撃１取得後、自動斬撃２が出るためなどに必要
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Experience")
	TArray<FName> AcquiredUpgradeIDs;

	/** レベルアップした時の処理を行う関数 */
	void LevelUp();

	//LevelUp関数で飛ばれる、レベルアップ時強化先選択のUI表示
	UFUNCTION(BlueprintImplementableEvent, Category = "LevelUp")
	void OnLevelUp();//ブルプリ上にあるから大丈夫

	// レベルアップUIのインスタンスを保持するための変数
	UPROPERTY()
	TObjectPtr<UUserWidget> LevelUpWidgetInstance;


	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LevelUpWidgetClass;

	//上はデータテーブルから選ばれた３つの強化先を入れるのに対してこっちは
	//アップグレード項目の名前が入る
	//この名前に応じて呼び出す強化処理が行われる。
	UFUNCTION(BlueprintCallable,Category="LevelUp")
	void ApplyUpgradeByID(FName UpgradeID);
	
	// 提示しているアップグレードの「行の名前」のリスト
//これがないとアップグレードの
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LevelUp")
	TArray<FName> OfferedUpgradeIDs;

	// レベルアップの選択肢が格納されているデータテーブル
	UPROPERTY(EditAnywhere, Category = "LevelUp")
	UDataTable* UpgradesDataTable;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

