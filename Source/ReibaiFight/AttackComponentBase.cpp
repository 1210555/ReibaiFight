#include "AttackComponentBase.h"

UAttackComponentBase::UAttackComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttackComponentBase::BeginPlay()
{
	Super::BeginPlay();
}

void UAttackComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction * ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// PerformAttack�̖{�́B���g�͋�ŁA�q�N���X�ɂ���ď㏑������邱�Ƃ�z��B
void UAttackComponentBase::PerformAttack()
{
	// This function is intended to be overridden by child classes
}

//���̃N���X���p�������N���X�i�܂�U���n�̎q�N���X�j�œ��e������
void UAttackComponentBase::Upgrade(float ModificationValue)
{
	// ��{�N���X�ł͉������Ȃ��B�q�N���X�����̒��g����������B
}