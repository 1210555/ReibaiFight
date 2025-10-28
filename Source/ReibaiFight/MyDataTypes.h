#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MyDataTypes.generated.h" // ファイル名に合わせて変更

// 前方宣言
class UAttackComponentBase;

// どんな種類のアップグレードかを定義する列挙型(enum)
UENUM(BlueprintType)
enum class EUpgradeType : uint8
{
    // 新しい攻撃コンポーネントを追加する
    AddNewAttack,
    // プレイヤーのステータス（移動速度など）を強化する
    ModifyPlayerStat,
    // 既存の攻撃のステータス（ダメージなど）を強化する
    ModifyAttackStat
};

//強化（アップグレード）データを定義する構造体
USTRUCT(BlueprintType)
struct FUpgradeData : public FTableRowBase
{
    GENERATED_BODY()

public:
    //UIに表示用のデータテーブルの各要素

    //強化先のタイトル、ちょっとでっかく
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText UpgradeName;

    //強化先の概要、説明。タイトルより小さく
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText UpgradeDescription;

    //強化先のアイコン
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TObjectPtr<UTexture2D> UpgradeIcon;

    //行の名前。これで自動斬撃3,4のような強化版を入手するロジックに必要
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName PrerequisiteID;

public:
    //強化の定義をここに追加していく

    // このアップグレードの種類
	//FUpgradeDataの上で定義しているenum classがEUpgradeType
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EUpgradeType Type;

    // (AddNewAttack用) 追加する攻撃コンポーネントのブループリント
	//UAttackComponentBaseを親に持つクラスのブループリントを格納できる変数
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    TSubclassOf<UAttackComponentBase> AttackComponentClass;

    // (ModifyPlayerStat用) 変更するステータスの名前
	//体力回復などはここに"MaxHealth"が入る
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName PlayerStatToModify;

    // (ModifyAttackStat用) 強化する攻撃コンポーネントのタグ
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName AttackComponentTag;

    // 強化する量
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    float ModificationValue;

};