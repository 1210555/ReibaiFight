#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MyDataTypes.generated.h" // �t�@�C�����ɍ��킹�ĕύX

// �O���錾
class UAttackComponentBase;

// �ǂ�Ȏ�ނ̃A�b�v�O���[�h�����`����񋓌^(enum)
UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
    // �V�����U���R���|�[�l���g��ǉ�����
    AddNewAttack,
    // �v���C���[�̃X�e�[�^�X�i�ړ����x�Ȃǁj����������
    ModifyPlayerStat,
    // �����̍U���̃X�e�[�^�X�i�_���[�W�Ȃǁj����������
    ModifyAttackStat
};

//�����i�A�b�v�O���[�h�j�f�[�^���`����\����
USTRUCT(BlueprintType)
struct FUpgradeData : public FTableRowBase
{
    GENERATED_BODY()

public:
    //UI�ɕ\���p�̃f�[�^�e�[�u���̊e�v�f

    //������̃^�C�g���A������Ƃł�����
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText UpgradeName;

    //������̊T�v�A�����B�^�C�g����菬����
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText UpgradeDescription;

    //������̃A�C�R��
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> UpgradeIcon;

    //�s�̖��O�B����Ŏ����a��3,4�̂悤�ȋ����ł���肷�郍�W�b�N�ɕK�v
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName PrerequisiteID;

public:
    //�����̒�`�������ɒǉ����Ă���

    // ���̃A�b�v�O���[�h�̎��
	//FUpgradeData�̏�Œ�`���Ă���enum class��EUpgradeType
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EUpgradeType Type;

    // (AddNewAttack�p) �ǉ�����U���R���|�[�l���g�̃u���[�v�����g
	//UAttackComponentBase��e�Ɏ��N���X�̃u���[�v�����g���i�[�ł���ϐ�
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UAttackComponentBase> AttackComponentClass;

    // (ModifyPlayerStat�p) �ύX����X�e�[�^�X�̖��O
	//�̗͉񕜂Ȃǂ͂�����"MaxHealth"������
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName PlayerStatToModify;

    // (ModifyAttackStat�p) ��������U���R���|�[�l���g�̃^�O
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName AttackComponentTag;

    // ���������
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ModificationValue;

};