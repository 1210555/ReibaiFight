#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PIDTrackingComponent.h" // PIDコンポーネントの設計図を先に読み込む
#include "NiagaraComponent.h"// Niagaraエフェクトの設計図も読み込む
#include "MyDataTypes.h"//AttackComponentBaseを継承していないため(Actorを親クラスとしているため)個別にインクルードが必要
#include "ReibaiFightBFL.h"//攻撃を与えるApplyDamage関数を使うために必要
#include "HitodamaBase.generated.h"


UCLASS()
class REIBAIFIGHT_API AHitodamaBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHitodamaBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// このアクターの「動きの脳」となるPIDコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UPIDTrackingComponent> PIDTrackingComponent;

	// このアクターの「見た目」となるNiagaraコンポーネント
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> NiagaraComponent;

	float BaseDamage = 20.0f;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//プーリングのための「起動」関数（C++とBPから呼べるようにする）
	UFUNCTION(BlueprintCallable, Category = "Hitodama")
	virtual void Activate(USceneComponent* TargetToFollow);

	//プーリングのための「停止（凍結）」関数
	UFUNCTION(BlueprintCallable, Category = "Hitodama")
	virtual void Deactivate();

	//強化用関数
	//ひとだまのアップデートはこの関数で行う
	virtual void Upgrade(const FUpgradeData& UpgradeData);

	//タグを持っているかチェックする便利関数
	//このタグはAttackComponentTagと比較される
	bool HasMatchingTag(FName TagToCheck) const;

	//PIDコンポーネントに角度をセットする関数
	//NewAngleは人玉の初期角度
	void SetOrbitAngle(float NewAngle);

	//ダメージを与える関数
	UFUNCTION(BlueprintCallable, Category="Attack")
	void PerformHitodamaAttack();

	//ひとだま攻撃の当たり判定の半径
	//見た目よりも少し大きくしておく
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack")
	float AttackRadius = 100.0f; // デフォルト値を設定

	// SphereComponent のポインタを宣言（UCLASS() の前に Component を定義しているはず）
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class USphereComponent* SphereCollision;
};
