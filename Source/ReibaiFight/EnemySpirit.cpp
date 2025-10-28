// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemySpirit.h"
#include "Kismet/GameplayStatics.h"//ApplyDamge�̂��߂ɕK�v
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Kismet/KismetSystemLibrary.h" //For SphereOverlapActors
#include "ReibaiFightCharacter.h"       //Include player character header
#include "DrawDebugHelpers.h"           //For debug sphere
#include "ReibaiFightBFL.h"//���색�C�u�����̃C���N���[�h

// Sets default values
AEnemySpirit::AEnemySpirit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
    AttackEnemyDamage = 12.0f;
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
    UE_LOG(LogTemp, Error, TEXT("ENEMY TakeDamage called!"))
    float DamageApplied = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

    // CurrentHealth�͎��O��EnemySpirit�N���X�Œ�`���Ă���
    CurrentHealth -= DamageApplied;

    if (CurrentHealth <= 0.0f)
    {
        // �u��𕥂��v���o
        // �����Ƀp�[�e�B�N���G�t�F�N�g��T�E���h���Đ����鏈��������

        //�A�N�^�[�����[���h������ł�����
		// ���S���̃A�j���[�V�����Đ�
        Die();
    }
    else {
        // �_���[�W���󂯂��ۂ̃��A�N�V�����i��: �t���b�V���G�t�F�N�g��T�E���h�j
		// �����Ƀ��A�N�V�����̏���������
    }

    return DamageApplied;
}

void AEnemySpirit::Die()
{
    // 1. AI�̎v�l���~������
    AAIController* AIController = Cast<AAIController>(GetController());
    if (AIController)
    {
        AIController->GetBrainComponent()->StopLogic("Dead");
    }

    // 2. �����蔻��������āA���̃L�����N�^�[�̎ז��ɂȂ�Ȃ��悤�ɂ���
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetCharacterMovement()->DisableMovement();

	// 3. �o���l�A�C�e���h���b�v�̏���
    if (ExperienceParticleClass)
    {
        FVector SpawnLocation = GetActorLocation();
        FRotator SpawnRotation = GetActorRotation();

        GetWorld()->SpawnActor<AActor>(ExperienceParticleClass, SpawnLocation, SpawnRotation);
    }

    // 4. ���S�A�j���[�V�������Đ�����
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
    if (AnimInstance && DeathMontage)
    {
        AnimInstance->Montage_Play(DeathMontage);
    }

    // 5. �A�j���[�V�����̒����ɍ��킹�āA������ɏ��ł�����
    SetLifeSpan(3.0f); // 3�b��ɂ��̃A�N�^�[�������I�ɏ��ł�����

    //���S���̃A�j���[�V�����Ȃ��Ă���������
    //���̂܂܂��ƃX�|�[���A�f�X�g���C���J��Ԃ��d���Ȃ�B
}

void AEnemySpirit::Attack()
{
	//�O��܂ł̍U���Ńq�b�g�����A�N�^�[�̃��X�g���N���A(���i�q�b�g�h�~�p)
    HitActorsDuringAttack.Empty();

	//�A�j���[�V�����C���X�^���X���擾
    UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

    //�A�j���[�V�����C���X�^���X�ƍĐ����ׂ��U�������^�[�W�������݂��邩�m�F
    if (AnimInstance&&AttackMontage)
    {
		//�U�������^�[�W�����Đ����łȂ��ꍇ�ɂ̂ݍĐ�
        //�߂��Ƃ��ɂ����ƍU�����[�V���������邱�Ƃ�h��
        if (AnimInstance && !AnimInstance->Montage_IsPlaying(AttackMontage))
        {
            UE_LOG(LogTemp, Log, TEXT("Enemy playing Attack Montage"));
            AnimInstance->Montage_Play(AttackMontage);
        }
	}
    else
    {
        //�؂蕪���p���O
        if(!AnimInstance)
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy Attack(): AnimInstance is null"));
		}
        if (!AttackMontage)
        {
            UE_LOG(LogTemp, Warning, TEXT("Enemy Attack(): AttackMontage is not set in Blueprint"));
        }
	}
}

void AEnemySpirit::AttackHitCheck(float DamageAmount)
{
    UE_LOG(LogTemp, Log, TEXT("Enemy AttackHitCheck CALLED !"));

    // --- �G���g�̍U���̒��S�ʒu�Ɣ��a���v�Z ---
    const FVector HitOrigin = GetActorLocation() + GetActorForwardVector() * 100.0f; // ��: �����̑O��100���j�b�g
    const float HitRadius = 100.0f; // ��: �G�̍U�����a

    UReibaiFightBFL::ApplyRadialDamage(
        this,
        HitOrigin,
        HitRadius,
        AttackEnemyDamage,
        HitActorsDuringAttack // <- �n���I
    );
    DrawDebugSphere(GetWorld(), HitOrigin, HitRadius, 12, FColor::Orange, false, 2.0f);
}