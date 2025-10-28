// Fill out your copyright notice in the Description page of Project Settings.


#include "ReibaiFightBFL.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnemySpirit.h" // �G�N���X�̃w�b�_�[

//�_���[�W��^����֐��A������
//(�U���ҁA�U���̒��S�ʒu�A�U���̃T�C�Y�i���a�j�A�^����_���[�W)
void UReibaiFightBFL::ApplyRadialDamage(AActor* Attacker, FVector Origin, float Radius, float BaseDamage, TSet<AActor*>& HitActors)
{

	UE_LOG(LogTemp, Warning, TEXT("BFL ApplyRadialDamage Started! Attacker: %s"), *Attacker->GetName());

	if (!Attacker) return;

	//�U���҂��v���C���[���G���𔻒�
	bool bAttackerIsPlayer = Attacker->IsA(AReibaiFightCharacter::StaticClass());

	//�_���[�W��^����Ώۂ̃N���X������
	UClass* ClassToTarget = bAttackerIsPlayer ? AEnemySpirit::StaticClass() : AReibaiFightCharacter::StaticClass();

	//��������A�N�^�[�̃��X�g�i�U���Ҏ��g�j
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(Attacker);

	//���o����A�N�^�[�̔z��
	TArray<AActor*> OverlappingActors;

	//�w�肵���ʒu�ŋ���̃I�[�o�[���b�v��������s
	bool bIsHit = UKismetSystemLibrary::SphereOverlapActors(
		Attacker->GetWorld(),
		Origin,
		Radius,
		{ UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn) },
		ClassToTarget,
		ActorsToIgnore,
		OverlappingActors
	);

	if (bIsHit)
	{
		for (AActor* HitActor : OverlappingActors)
		{
			if (!HitActors.Contains(HitActor))
			{
				//�N�Ƀ_���[�W�^������
				UE_LOG(LogTemp, Warning, TEXT("BFL Applying Damage to: %s"), *HitActor->GetName());

				// �q�b�g�����G�Ƀ_���[�W��^����
				UGameplayStatics::ApplyDamage(
					HitActor,
					BaseDamage,
					Attacker->GetInstigatorController(), // �U���҂̃R���g���[���[���擾
					Attacker,
					UDamageType::StaticClass()
				);
				HitActors.Add(HitActor); //���i�q�b�g�h�~�̂��߂̃q�b�g���X�g�ɒǉ�
			}
		}
	}
}

AEnemySpirit* UReibaiFightBFL::GetNearestEnemy(const UObject* WorldContextObject, FVector Origin, float Radius)
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(WorldContextObject, AEnemySpirit::StaticClass(), FoundActors);

	AActor* NearestActor = nullptr;
	float MinDistance = Radius;

	for (AActor* Actor : FoundActors)
	{
		float Distance = FVector::Dist(Origin, Actor->GetActorLocation());
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			NearestActor = Actor;
		}
	}

	return Cast<AEnemySpirit>(NearestActor);
}