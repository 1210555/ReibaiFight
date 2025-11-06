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
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "メインの強化名"))
    FText UpgradeName;

    //強化先の概要、説明。タイトルより小さく
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "強化の内容、説明"))
    FText UpgradeDescription;

    //強化先のアイコン
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "強化のアイコン画像"))
    TObjectPtr<UTexture2D> UpgradeIcon;

    //行の名前。これで自動斬撃3,4のような強化版を入手するロジックに必要
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "事前に有していなければならない '行' の名前。レベル1,2…のように上がっていくものは必要"))
    FName PrerequisiteID;

    // このアップグレードの種類
	//FUpgradeDataの上で定義しているenum classがEUpgradeType
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "新しい攻撃の追加、体力回復・身体強化、既存の攻撃の攻撃の強化"))
    EUpgradeType Type;

    // (AddNewAttack用) 追加する攻撃コンポーネントのブループリント
	//UAttackComponentBaseを親に持つクラスのブループリントを格納できる変数
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "ここで設定するBPをもとにアタックコンポーネント生成"))
    TSubclassOf<UAttackComponentBase> AttackComponentClass;

    // (ModifyPlayerStat用) 変更するステータスの名前
	//体力回復などはここに"MaxHealth"が入る
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "ModifyPlayerStatの値を変化する場合何の値を変更するかを記述"))
    FName PlayerStatToModify;

    // (ModifyAttackStat用) 強化する攻撃コンポーネントのタグ
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "BPで設定しているタグと一致しているものをレベルアップ時に探してくる"))
    FName AttackComponentTag;

    // 強化する量
    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "攻撃の威力をどれだけ増やすか(float型)"))
    float ModificationAttackAmount;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Tooltip = "攻撃の頻度をどれだけ短縮するか(記述した % だけ短縮)"))
    float ModificationFrequentValue;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Tooltip = "体力の回復などModifyPlayerStatの値をどれだけ変更するか"))
    float ModificationValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,meta = (Tooltip = "何度でも取得可能なアップグレードはtrueにする"))
	bool bIsRepeatable;
};