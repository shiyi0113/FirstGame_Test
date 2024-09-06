#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Delegates/DelegateCombinations.h"
#include "GameEnemy.generated.h"

class UArrowComponent;
class UAnimMontage;
class UBoxComponent;
class AMyGameMode;
class AGamePlayer;
class UWidgetComponent;
class UMetaSoundSource;
UCLASS()
class FIRSTGAME_MYTEST_API AGameEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	AGameEnemy();
	virtual void Tick(float DeltaTime) override;
	//受伤
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Transformation)
	UArrowComponent* AttackArrow;                   //箭头组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Transformation)
	UBoxComponent* AttackBox;                       //碰撞盒组件
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* AttackMontage;                    //攻击动画
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI") 
	UWidgetComponent* HealthBarComponent;           //血条
	UMetaSoundSource* DamageAudio;                  //受击音效
	UPROPERTY(EditAnywhere, Category = "Enemy Attribute")
	float MaxHealth;               //最大生命值
	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth;           //当前生命值
	UPROPERTY(EditAnywhere, Category = "Enemy Attribute")
	float Damage;
	AGamePlayer* TargetActor;      //主角
	AMyGameMode* MyGameMode;       //GameMode
	UFUNCTION(BlueprintCallable)
	void Attack();
	void PerformAttack();
private:
	void MoveToTarget();
	bool CanAttack;
	bool EnemyIsDie;
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted); //控制攻击蒙太奇
	void UpdateHealthBar();
};
