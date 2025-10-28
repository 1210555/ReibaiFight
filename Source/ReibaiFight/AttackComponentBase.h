#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackComponentBase.generated.h"

UCLASS(Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class REIBAIFIGHT_API UAttackComponentBase : public UActorComponent
{
	GENERATED_BODY()

public:
	UAttackComponentBase();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// C++�ƃu���[�v�����g�̗�������Ăяo����U�����s�֐�
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void PerformAttack(); // C++�̎q�N���X��override���邽�߂� 'virtual' �֐�

	// �u���[�v�����g���ŃC�x���g�Ƃ��Ď������邽�߂̊֐� (����͎g���܂��񂪁A��������������������܂�)
	// UFUNCTION(BlueprintNativeEvent, Category = "Combat")
	// void PerformAttack_Blueprint();

	//�q�N���X�ŃA�b�v�O���[�h������
	virtual void Upgrade(float ModificationValue);
};