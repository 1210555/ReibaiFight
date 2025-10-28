// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "AttackComponentBase.h"
#include "MyDataTypes.h"
#include "ReibaiFightCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AReibaiFightCharacter : public ACharacter
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

	//�U���̓��͂�����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StrongPunch1Action;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* Punch1Action;

public:
	AReibaiFightCharacter();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxSpiritPower = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentSpiritPower;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	//�ǂ�WBP�QHUD���g�������w�肷�邽�߂̕ϐ�
	//���x����ɔz�u�����L�����N�^�[�C���X�^���X���ƁA��������BP�̃f�t�H���g�ݒ�ł��ҏW�ł���悤��
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	//HUD�E�B�W�F�b�g�̃C���X�^���X��ێ����邽�߂̕ϐ�
	//�K�x�[�W�R���N�V�����Ɉ���������Ȃ��悤�ɂ��邽�߂�UPROPERTY������
	UPROPERTY()
	TObjectPtr<UUserWidget> HUDWidgetInstance;

protected:
	//������void�֐���UE�G�f�B�^���Ŏg�p�ł��Ȃ��A�����̌v�Z�Ȃǂ��s���Ƃ��͂�����void�ɂ���B

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	void Attack(const FInputActionValue& Value);

	void Punch1(const FInputActionValue& Value);

	//�U���̃A�j���[�V����
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
	UAnimMontage* StrongPunch1Montage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	UAnimMontage* Punch1Montage;

	//�U���̒ʒm���������Ƀu���[�v�����g��ŌĂяo��
	UFUNCTION(BlueprintCallable, Category = "Combat",meta= (ToolTip = "for player")) // Anim Notify����Ăяo����悤�ɂ���
	virtual void AttackHitCheck(float DamageAmount);

	// 1��̍U���Ŋ��Ƀq�b�g�����A�N�^�[���i�[����z��(���i�q�b�g�h�~)
	TSet<AActor*> HitActors;

	// �����U�����L�����ǂ����̃t���O
	//���ꂪtrue�̂Ƃ����������U�����s��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	bool bHasAutoAttack = false;

	// ���ݑ������Ă���U���R���|�[�l���g
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	UAttackComponentBase* CurrentAttackComponent;

	// �����U���̃^�C�}�[�n���h��
	FTimerHandle AutoAttackTimerHandle;

	/** �����U�����J�n����֐��i�u���[�v�����g����Ăяo�����߂̂��́j */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EnableAutoAttack();

	/** �^�C�}�[�ɂ���Ē���I�ɌĂяo�����֐� */
	void TriggerAutoAttack();

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Experience")
	int32 CurrentLevel = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Experience")
	int32 CurrentXP = 0;

	//���̃��x���܂łɕK�v�Ȍo���l
	// 1��2 �̂Ƃ��� 100 , 2��3 �̂Ƃ��� 100 * 1.5 , 3��4 �̂Ƃ���100 * 1.5 * 1.5 �c
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Experience")
	int32 XPToNextLevel = 100;

	/* �o���l���l������֐��A���x���A�b�v����BP����Ă΂��*/
	UFUNCTION(BlueprintCallable, Category = "Experience")
	void GainExperience(int32 XPAmount);

	// �v���C���[�����Ɏ擾�����A�b�v�O���[�h��ID���i�[����z��
	//�����a���P�擾��A�����a���Q���o�邽�߂ȂǂɕK�v
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Experience")
	TArray<FName> AcquiredUpgradeIDs;

	/** ���x���A�b�v�������̏������s���֐� */
	void LevelUp();

	//LevelUp�֐��Ŕ�΂��A���x���A�b�v��������I����UI�\��
	UFUNCTION(BlueprintImplementableEvent, Category = "LevelUp")
	void OnLevelUp();//�u���v����ɂ��邩����v

	// ���x���A�b�vUI�̃C���X�^���X��ێ����邽�߂̕ϐ�
	UPROPERTY()
	TObjectPtr<UUserWidget> LevelUpWidgetInstance;

	// �u���[�v�����g����Ăяo�����߂̊֐�
	UFUNCTION(BlueprintCallable, Category = "LevelUp")
	void ApplyUpgrade(FName UpgradeID);

	// 
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> LevelUpWidgetClass;

	//��̓f�[�^�e�[�u������I�΂ꂽ�R�̋����������̂ɑ΂��Ă�������
	//�A�b�v�O���[�h���ڂ̖��O������
	//���̖��O�ɉ����ČĂяo�������������s����B
	UFUNCTION(BlueprintCallable,Category="LevelUp")
	void ApplyUpgradeByID(FName UpgradeID);
	
	// �񎦂��Ă���A�b�v�O���[�h�́u�s�̖��O�v�̃��X�g
//���ꂪ�Ȃ��ƃA�b�v�O���[�h��
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "LevelUp")
	TArray<FName> OfferedUpgradeIDs;

	// ���x���A�b�v�̑I�������i�[����Ă���f�[�^�e�[�u��
	UPROPERTY(EditAnywhere, Category = "LevelUp")
	UDataTable* UpgradesDataTable;

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

