#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GamePlayer.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
class UArrowComponent;
struct FInputActionValue;

UCLASS()
class FIRSTGAME_MYTEST_API AGamePlayer : public ACharacter
{
	GENERATED_BODY()

public:
	AGamePlayer();
	void PerformAttack();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	//���˺���
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;

public:
	USpringArmComponent* CameraArm;                 //���ɱ�
	UCameraComponent* FollowCamera;                 //�����
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Transformation)
	UArrowComponent* AttackArrow;                   //��ͷ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;    //����ӳ��������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;                       //��Ծ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;                       //�ƶ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;                       //��ͷת��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;                     //����

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* AttackMontage;                    //��������

protected:
	void Move(const FInputActionValue& Value);  
	void Look(const FInputActionValue& Value);
	void Jump();
	void Attack();
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted); //����Attack�����������깥���������ܽ�����һ�ι���
 
public:
	UPROPERTY(EditAnywhere, Category = "GamePlayer Attribute")
	float MaxHealth;               //�������ֵ
	float CurrentHealth;           //��ǰ����ֵ
	UPROPERTY(EditAnywhere, Category = "GamePlayer Attribute")
	float Damage;                  //�˺�
	
private:
	bool CanAttack;                //������������
	TArray<AActor*> DamagedActors; //�洢���ι����Ѿ����˵ĵ���
};
