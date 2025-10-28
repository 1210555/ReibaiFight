#include "AutoSlashAttack.h"
#include "GameFramework/Character.h"
#include "Animation/AnimInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnemySpirit.h" // �G�N���X�̃w�b�_�[
#include "NiagaraFunctionLibrary.h"
#include "ReibaiFightBFL.h"


void UAutoSlashAttack::PerformAttack()
{
	// ���������L���Ă���A�N�^�[�i�v���C���[�L�����N�^�[�j���擾���܂�
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;

	// 1. �ł��߂��G��T���܂�
	AActor* TargetEnemy = UReibaiFightBFL::GetNearestEnemy(
		GetWorld(),//
		OwnerCharacter->GetActorLocation(),//�T�����_�i�v���C���[�̍��W�j
		3000.0f//�T�����a
	);

	// 2. �G�����������ꍇ
	if (TargetEnemy)
	{
		// 3. �G�̂ق��������擾(�R�����g�A�E�g�O���ΓG�̕����ނ����Ⴄ)
		FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(OwnerCharacter->GetActorLocation(), TargetEnemy->GetActorLocation());
		//OwnerCharacter->SetActorRotation(FRotator(0.f, LookAtRotation.Yaw, 0.f));

		// 4. �G�t�F�N�g���Đ����܂�
		if (SlashEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				SlashEffect,
				TargetEnemy->GetActorLocation(), // �� �G�t�F�N�g���o���i�G�̈ʒu�j
				LookAtRotation//�G�̕����ɍU��������������
			);
		}

		// 5. �U�������^�[�W�����Đ����܂�
		UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
		if (AnimInstance && SlashMontage)
		{
			AnimInstance->Montage_Play(SlashMontage);
		}

		//����̍U���Ńq�b�g�����G���L�^���邽�߂́A��̃��X�g�����
		TSet<AActor*> HitActorsInThisAttack;

		// �����蔻��̃��W�b�N�́A���C�u�����̊֐����Ăяo�������ł���
		UReibaiFightBFL::ApplyRadialDamage(
			OwnerCharacter,                 // �U����
			TargetEnemy->GetActorLocation(),// �_���[�W�̒��S�_�i�G�̈ʒu�j
			150.0f,                         // �_���[�W�͈͂̔��a
			BaseDamage,                     // �_���[�W��
			HitActorsInThisAttack           // ���̍U���Ńq�b�g�����A�N�^�[�̃��X�g
		);
	}
}

void UAutoSlashAttack::Upgrade(float ModificationValue)
{
	// �ȑO�̎������Ăяo��
	Super::Upgrade(ModificationValue);

	// ���̃R���|�[�l���g������Damage�ϐ��𑝂₷�ȂǁA��̓I�ȋ��������������ɏ���
	BaseDamage += ModificationValue;
	UE_LOG(LogTemp, Warning, TEXT("AutoSlash Attack Upgraded! New Damage: %f"), BaseDamage);
}