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
#include "Kismet/GameplayStatics.h"//ApplyDamage�ɕK�v
#include "Kismet/KismetMathLibrary.h"
#include "EnemySpirit.h"
#include "Animation/AnimInstance.h"      // AnimInstance���g���ꍇ�ɕK�v
#include "DrawDebugHelpers.h"            // �f�o�b�O�\���ɕK�v
#include "Blueprint/UserWidget.h" // UUserWidget���g���ꍇ�ɕK�v
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

	//���蓖�Ă��s����������
	//IMC Default�ɒǉ�����
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// DefaultMappingContext �� .h �t�@�C���� UPROPERTY �Ƃ��Đ錾���Ă����K�v������܂�
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input
//�L�����N�^�[���v���C���[�ɑ��삳��n�߂�ŏ��̈�񂾂��Ă΂��
//�����ݒ�p�̊֐�
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
		
//BindAction�ł�
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AReibaiFightCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AReibaiFightCharacter::Look);
		UE_LOG(LogTemp, Warning, TEXT("Attack Action. SetupPlayerInoutComponent"));
		//�U���̓���
		EnhancedInputComponent->BindAction(StrongPunch1Action, ETriggerEvent::Triggered, this, &AReibaiFightCharacter::Attack);

		//Punch1Action�Ƃ������O�̓��͂���������&AReibaiFightCharacter::Punch1�Ƃ����֐����Ăяo��
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
	//���ꂪ�Ȃ��Ƒ��i�q�b�g�h�~�̂��߂�HitActors�Ɉ�x�����͂��炸
	//2��ڈȍ~�̍U����������Ȃ��Ȃ�
	HitActors.Empty(); //�V�����U���̂��тɃq�b�g�����A�N�^�[�̃��X�g���N���A
	// input is a bool
	UE_LOG(LogTemp, Warning, TEXT("Attack"));
	bool bPressed = Value.Get<bool>();
	if (bPressed)
	{
		//�U���A�j���[�V�������Đ�����
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
	HitActors.Empty(); //�V�����U���̂��тɃq�b�g�����A�N�^�[�̃��X�g���N���A
	// ���O���o���āA�֐����Ă΂ꂽ���m�F
	UE_LOG(LogTemp, Warning, TEXT("Punch1 function called!"));

	bool bPressed = Value.Get<bool>();
	if (bPressed)
	{
		// Punch1�̃A�j���[�V�������Đ�����
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

	// --- �����蔻��̒��S�ʒu���v�Z ---
	// ���̃\�P�b�g�ʒu���擾 (�\�P�b�g�� "Fist_R_Socket" �͗v�m�F)
	const FVector HitOrigin = GetMesh()->GetSocketLocation("Fist_R_Socket");
	const float HitRadius = 80.0f; // ���̍U���̔��a

	// --- BFL�̊֐����Ăяo���ă_���[�W�K�p ---
	// HitActors �� .h �Œ�`���� TSet<AActor*> �^�̃����o�[�ϐ�
	UReibaiFightBFL::ApplyRadialDamage(
		this,        // �U���҂͎������g
		HitOrigin,   // ���̈ʒu
		HitRadius,   // �ݒ肵�����a
		DamageAmount,// ���̍U���̃_���[�W�� (AnimNotify����n�����)
		HitActors    // ���̍U���Ńq�b�g�ς݂̃��X�g
	);

	// �f�o�b�O�p�ɓ����蔻��̋������o��
	DrawDebugSphere(GetWorld(), HitOrigin, HitRadius, 12, FColor::Red, false, 2.0f);
}

float AReibaiFightCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Warning, TEXT("Player TakeDamage Started! DamageAmount: %f"), DamageAmount);
	// �܂��e�N���X��TakeDamage���Ăяo���āA�ŏI�I�ȃ_���[�W�ʂ��v�Z���Ă��炤 (�d�v�I)
	float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// �̗͂��c���Ă���΃_���[�W��K�p
	if (CurrentHealth > 0.0f)
	{
		CurrentHealth -= DamageApplied;
		UE_LOG(LogTemp, Warning, TEXT("Player took %f damage. Current Health: %f"), DamageApplied, CurrentHealth);

		if (HUDWidgetInstance) // HUD�E�B�W�F�b�g�ւ̎Q�Ƃ����݂��邩�`�F�b�N
		{
			// 1. �Ăяo���C�x���g�����w��
			FName FuncName = FName("UpdateHealthBar");

			// 2. �E�B�W�F�b�g���炻�̖��O�̊֐�(�C�x���g)��T��
			UFunction* Function = HUDWidgetInstance->FindFunction(FuncName);

			// 3. �֐�������������
			if (Function)
			{
				// 4. �C�x���g�ɓn���������������邽�߂̍\����
				struct { float Current; float Max; } Params;
				Params.Current = CurrentHealth; // ���݂̗̑͂��Z�b�g
				Params.Max = MaxHealth;       // �ő�̗͂��Z�b�g

				// 5. �C�x���g�����s���A������n��
				HUDWidgetInstance->ProcessEvent(Function, &Params);
			}
		}

		// �̗͂��[���ȉ��ɂȂ������`�F�b�N
		if (CurrentHealth <= 0.0f)
		{
			// TODO: �v���C���[�̎��S���� (��: �Q�[���I�[�o�[��ʕ\���A���X�^�[�g�Ȃ�)
			UE_LOG(LogTemp, Error, TEXT("PLAYER DIED!"));
			// DisableInput(GetController<APlayerController>()); // ����s�\�ɂ���Ȃ�
		}
		else
		{
			// TODO: �v���C���[�̔�_���[�W���A�N�V���� (��ʂ̓_�ŁA�T�E���h�Đ��Ȃ�)
		}
	}

	return DamageApplied; // ���ۂɓK�p���ꂽ�_���[�W�ʂ�Ԃ�
}

//�u���v������Ăяo���i�����U���J�n�̍��}�j
void AReibaiFightCharacter::EnableAutoAttack()
{
	// ���łɎ����U�����L���ȏꍇ�͉������Ȃ�
	if (bHasAutoAttack) return;

	bHasAutoAttack = true;

	// �����ɒǉ�����Ă���U���R���|�[�l���g��T���o��
	CurrentAttackComponent = FindComponentByClass<UAttackComponentBase>();

	if (CurrentAttackComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("Auto Attack Enabled!"));

		// �^�C�}�[���J�n����
		GetWorld()->GetTimerManager().SetTimer(
			AutoAttackTimerHandle,
			this,
			&AReibaiFightCharacter::TriggerAutoAttack,
			2.0f, // 2�b���ƂɎ��s
			true  // �J��Ԃ�
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("EnableAutoAttack failed: No Attack Component found!"));
	}
}

//�^�C�}�[�Œ���I�ɍs���鏈��
void AReibaiFightCharacter::TriggerAutoAttack()
{
	if (CurrentAttackComponent)
	{
		// �������Ă���U���R���|�[�l���g�ɍU�������s������
		CurrentAttackComponent->PerformAttack();
	}
}

void AReibaiFightCharacter::GainExperience(int32 XPAmount)
{
	CurrentXP += XPAmount;
	UE_LOG(LogTemp, Warning, TEXT("Gained %d XP. Current XP: %d / %d"), XPAmount, CurrentXP, XPToNextLevel);

	// �������݂̌o���l���K�v�o���l�𒴂����烌�x���A�b�v
	if (CurrentXP >= XPToNextLevel)
	{
		LevelUp();
	}
}

void AReibaiFightCharacter::LevelUp()
{
	// ���x���A�b�v����
	CurrentLevel++;

	// �]�����o���l�����̃��x���Ɏ����z��
	CurrentXP -= XPToNextLevel;

	// ���̃��x���ɕK�v�Ȍo���l�𑝂₷�i��F1.5�{�ɂ���j
	XPToNextLevel = FMath::RoundToInt(XPToNextLevel * 1.5f);

	UE_LOG(LogTemp, Warning, TEXT("LEVEL UP! New Level: %d. Next XP: %d"), CurrentLevel, XPToNextLevel);


	if (UpgradesDataTable) // �f�[�^�e�[�u�����ݒ肳��Ă��邩�`�F�b�N
	{
		//�ȑO�̃��X�g���N���A
		//����Ȃ��ƃ��x���A�b�v��UI�ɂǂ�ǂ�ǉ�����Ă��܂�
		OfferedUpgradeIDs.Empty();

		//�f�[�^�e�[�u������S�Ă̍s�̖��O���擾
		TArray<FName> AllUpgradeIDs = UpgradesDataTable->GetRowNames();

		// TODO: �����Ɂu�擾�ς݂ł͂Ȃ��v�u�O������𖞂����Ă���v�Ȃǂ̃t�B���^�����O������ǉ�����

		// ���X�g���V���b�t��
		// �z����V���b�t�����ă����_���ȏ��Ԃɂ���
		const int32 Num = AllUpgradeIDs.Num();
		for (int32 i = 0; i < Num; ++i)
		{
			int32 j = FMath::RandRange(i, Num - 1);
			if (i != j)
			{
				AllUpgradeIDs.Swap(i, j);
			}
		}

		// �V���b�t���������X�g�̐擪����3��OfferedUpgradeIDs�ɒǉ�
		int32 NumToOffer = FMath::Min(3, AllUpgradeIDs.Num());
		for (int32 i = 0; i < NumToOffer; ++i)
		{
			OfferedUpgradeIDs.Add(AllUpgradeIDs[i]);
		}
	}

	// ���x���A�b�v��V�̑I��UI��\�����A�Q�[�����ꎞ��~���鏈�����Ăяo��
	OnLevelUp();
}

void AReibaiFightCharacter::ApplyUpgrade(FName UpgradeID)
{
	/*UE_LOG(LogTemp, Warning, TEXT("Upgrade Selected: %s"), *UpgradeID.ToString());

	// TODO: UpgradeID�ɉ����āA���ۂɔ\�͂�K�p���鏈���������ɏ���
	// (��: �V�����U���R���|�[�l���g��ǉ�����A�X�e�[�^�X���グ��Ȃ�)

	// --- �Q�[���ɖ߂鏈�� ---

	// 1. �Q�[���̈ꎞ��~������
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		PlayerController->SetPause(false);
		PlayerController->bShowMouseCursor = false;
		// ...
	}

	// 2. ���x���A�b�vUI����ʂ���폜
	if (LevelUpWidgetInstance)
	{
		LevelUpWidgetInstance->RemoveFromParent();
	}*/
}

//Index�̓��x���A�b�v���ɑI�������A�b�v�O���[�h�̔z����C���f�b�N�X(���x���A�b�v��UI�̍��̍��ڂ���O�C�P�C�Q������)
void AReibaiFightCharacter::ApplyUpgradeByID(FName UpgradeID)
{
	// OfferedUpgrades�z�񂪗L���ȃC���f�b�N�X���`�F�b�N
	//OfferedUpgrades�̓f�[�^�e�[�u���̍s�̖��O�̔z��
	// 
	//if (!OfferedUpgradeIDs.IsValidIndex(Index))//Index=0�̂Ƃ���ԍ��̃{�^���ɏ����ꂽ�A�b�v�O���[�h���ڂ̖��O������
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Invalid Upgrade Index: %d"), Index);
	//	return;
	//}

	//�I�����ꂽ�A�b�v�O���[�h�̍s�̖��O���擾
	//const FName UpgradeID = OfferedUpgradeIDs[Index];
	
	UE_LOG(LogTemp, Warning, TEXT("Upgrade Selected: %s"), *UpgradeID.ToString());

	//�擾�ς݃��X�g�ɒǉ�
	AcquiredUpgradeIDs.Add(UpgradeID);

	//�f�[�^�e�[�u������A�I�����ꂽ�A�b�v�O���[�h�̊��S�ȃf�[�^���擾
	FUpgradeData* UpgradeData = UpgradesDataTable->FindRow<FUpgradeData>(UpgradeID, TEXT(""));
	if (!UpgradeData)
	{
		//�f�[�^��������Ȃ������ꍇ�̃G���[����
		UE_LOG(LogTemp, Error, TEXT("Could not find UpgradeData for ID: %s"), *UpgradeID.ToString());
		// ... �Q�[�����ĊJ���鏈�� ...
		return;
	}

	//�擾����UpgradeData�\���̂�Type�ɉ����ď����𕪊�
	switch (UpgradeData->Type)
	{
	//�U���ǉ�
	case EUpgradeType::AddNewAttack:
		// ... (AddComponentByClass ���g�����R���|�[�l���g�ǉ�����) ...
		if(UpgradeData->AttackComponentClass)	//AttackComponentClass�ɐ݌v�}���w�肳��Ă��邩�H
		{
			//�w�肳�ꂽ�N���X�̃R���|�[�l���g�������ɒǉ�
			//�^��UActorComponent�̂��߈ȉ��ŃL���X�g���K�v
			UActorComponent* AddedComponent = AddComponentByClass(UpgradeData->AttackComponentClass, false, FTransform(), false);

			//�ǉ����ꂽ�R���|�[�l���g��AttackComponentBase�^�ɃL���X�g
			UAttackComponentBase* NewAttack = Cast<UAttackComponentBase>(AddedComponent);

			if (NewAttack)
			{
				NewAttack->RegisterComponent(); // �R���|�[�l���g��o�^
				UE_LOG(LogTemp, Warning, TEXT("Added new attack component: %s"), *NewAttack->GetName());
			}
		}
		break;
	//�̗͉�
	case EUpgradeType::ModifyPlayerStat:
		if (UpgradeData->PlayerStatToModify == "MaxHealth")
		{
			MaxHealth += UpgradeData->ModificationValue;
			CurrentHealth = MaxHealth;
		}
		break;
	//�����̔\�͋���
	case EUpgradeType::ModifyAttackStat:
		// �����Ώۂ̃R���|�[�l���g��T��
		TArray<UActorComponent*> Components;
		GetComponents(Components); // �����ɂ��Ă���S�R���|�[�l���g���擾
		for (UActorComponent* Component : Components)
		{
			//�w�肳�ꂽ�^�O�����R���|�[�l���g���H
			if (Component->ComponentHasTag(UpgradeData->AttackComponentTag))
			{
				//���Ɓi�U���R���|�[�l���g�j�Ɏd����C����
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


	// --- �Q�[���ɖ߂鏈�� ---

	// 1. �Q�[���̈ꎞ��~������
	APlayerController* PlayerController = GetController<APlayerController>();
	if (PlayerController)
	{
		PlayerController->SetPause(false);
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly()); // ���̓��[�h���Q�[���݂̂ɖ߂�
	}

	// 2. ���x���A�b�vUI����ʂ���폜
	if (LevelUpWidgetInstance)
	{
		LevelUpWidgetInstance->RemoveFromParent();
		LevelUpWidgetInstance = nullptr; // �ϐ����N���A
	}
}