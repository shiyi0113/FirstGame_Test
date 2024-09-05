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
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
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
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void PerformAttack();           
private:
	bool CanAttack;
	TArray<AActor*> DamagedActors; //�汾�ι����Ѿ����˵ĵ���
};
