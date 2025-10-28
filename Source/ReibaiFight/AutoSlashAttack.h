#pragma once

#include "CoreMinimal.h"
#include "AttackComponentBase.h"
#include "NiagaraSystem.h"
#include "AutoSlashAttack.generated.h"

UCLASS(Blueprintable)
class REIBAIFIGHT_API UAutoSlashAttack : public UAttackComponentBase
{
	GENERATED_BODY()

public:
	// �e�N���X��PerformAttack�֐����㏑�����܂�
	virtual void PerformAttack() override;

protected:
	// ���̍U���ōĐ����郂���^�[�W��
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* SlashMontage;

	// �Đ��������a���G�t�F�N�g
	UPROPERTY(EditAnywhere, Category = "VFX")
	UNiagaraSystem* SlashEffect;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float defaultSlashDamage = 60.0f;

private:
	// �ł��߂��G��T���w���p�[�֐�
	AActor* FindNearestEnemy();

protected:
	// ���̍U�����^�����{�_���[�W
	UPROPERTY(EditAnywhere, Category = "Combat")
	float BaseDamage = 25.0f;

public:
	// �e�N���X��Upgrade�֐����㏑��
	virtual void Upgrade(float ModificationValue) override;
};