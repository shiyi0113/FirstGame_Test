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
	//����
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Transformation)
	UArrowComponent* AttackArrow;                   //��ͷ���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Transformation)
	UBoxComponent* AttackBox;                       //��ײ�����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* AttackMontage;                    //��������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI") 
	UWidgetComponent* HealthBarComponent;           //Ѫ��
	UMetaSoundSource* DamageAudio;                  //�ܻ���Ч
	UPROPERTY(EditAnywhere, Category = "Enemy Attribute")
	float MaxHealth;               //�������ֵ
	UPROPERTY(BlueprintReadOnly)
	float CurrentHealth;           //��ǰ����ֵ
	UPROPERTY(EditAnywhere, Category = "Enemy Attribute")
	float Damage;
	AGamePlayer* TargetActor;      //����
	AMyGameMode* MyGameMode;       //GameMode
	UFUNCTION(BlueprintCallable)
	void Attack();
	void PerformAttack();
private:
	void MoveToTarget();
	bool CanAttack;
	bool EnemyIsDie;
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted); //���ƹ�����̫��
	void UpdateHealthBar();
};
