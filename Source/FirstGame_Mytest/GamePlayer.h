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
	USpringArmComponent* CameraArm;                 //弹簧臂
	UCameraComponent* FollowCamera;                 //摄像机
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Transformation)
	UArrowComponent* AttackArrow;                   //箭头组件
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* DefaultMappingContext;    //输入映射上下文
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* JumpAction;                       //跳跃
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* MoveAction;                       //移动
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;                       //镜头转动
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* AttackAction;                     //攻击

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimMontage* AttackMontage;                    //攻击动画
protected:
	void Move(const FInputActionValue& Value);  
	void Look(const FInputActionValue& Value);
	void Jump();
	void Attack();
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);
	void PerformAttack();           
private:
	bool CanAttack;
	TArray<AActor*> DamagedActors; //存本次攻击已经受伤的敌人
};
