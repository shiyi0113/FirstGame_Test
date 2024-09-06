#include "GamePlayer.h"
#include "GameEnemy.h"
#include "Engine/LocalPlayer.h"
#include "Engine/DamageEvents.h"
#include "Components/CapsuleComponent.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

AGamePlayer::AGamePlayer()
{
	PrimaryActorTick.bCanEverTick = false;
	//获取角色的胶囊体组件，并设置大小
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	//让角色不跟随控制器的俯仰角、偏移角、滚轮
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	//获取角色的角色移动组件
	GetCharacterMovement()->bOrientRotationToMovement = true;   //将旋转朝向运动
	GetCharacterMovement()->MaxWalkSpeed = 500.f;               //最大移动速度
	//弹簧臂组件
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 400.0f; 	                    //目标臂长度
	CameraArm->bUsePawnControlRotation = true;                  //使用Pawn控制旋转
	//摄像机
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);  //绑到弹簧臂末端
	FollowCamera->bUsePawnControlRotation = false;             //关闭使用Pawn控制旋转，因为弹簧臂控制了
	//箭头组件
	AttackArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	AttackArrow->SetupAttachment(RootComponent);
	//初始化一些属性
	CanAttack = true;
	MaxHealth = 300.0f;
	CurrentHealth = MaxHealth;
	Damage = 10.0f;
}

void AGamePlayer::BeginPlay()
{
	Super::BeginPlay();
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AGamePlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);   //转换成增强输入

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGamePlayer::Jump);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGamePlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGamePlayer::Look);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AGamePlayer::Attack);
}
//移动
void AGamePlayer::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}
//摄像机视角
void AGamePlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
//跳跃
void AGamePlayer::Jump()
{
	bPressedJump = true;
}
//播放攻击动画
void AGamePlayer::Attack()
{
	if (CanAttack && AttackMontage) {
		PlayAnimMontage(AttackMontage);
		CanAttack = false;
		FOnMontageEnded MontageEndedDelegate;   //创建一个委托
		MontageEndedDelegate.BindUObject(this, &AGamePlayer::OnAttackMontageEnded);  //将委托绑定到这个函数OnAttackMontageEnded
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndedDelegate, AttackMontage); //蒙太奇动画播放完后执行函数
	}
}

void AGamePlayer::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage) {
		CanAttack = true;
	}
}
//攻击出伤检测  （使用动画通知调用）
void AGamePlayer::PerformAttack()
{
	FVector Start = AttackArrow->GetComponentLocation();
	FVector End = Start + AttackArrow->GetForwardVector() * 50.0f;
	TArray<FHitResult> HitResults;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(30.0f);
	DrawDebugSphere(GetWorld(), Start, 30.0f, 12, FColor::Green, false, 1.0f);
	DrawDebugSphere(GetWorld(), End, 30.0f, 12, FColor::Red, false, 1.0f);
	DrawDebugLine(GetWorld(), Start, End, FColor::Blue, false, 1.0f, 0, 1.0f);
	bool bHit = GetWorld()->SweepMultiByObjectType(
		HitResults,
		Start,
		End,
		FQuat::Identity,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn),
		CollisionShape
	);
	if (bHit) {
		for (auto& Hit : HitResults) {
			AActor* HitActor = Hit.GetActor();
			if (HitActor && !DamagedActors.Contains(HitActor)) {
				AGameEnemy* Enemy = Cast<AGameEnemy>(HitActor);
				if (Enemy) {
					FDamageEvent DamageEvent;
					if (Enemy->CurrentHealth > 0) {
						Enemy->TakeDamage(Damage, DamageEvent, GetController(), this);
						DamagedActors.Add(HitActor);
					}
				}
			}
		}
	}
	DamagedActors.Empty();
}

float AGamePlayer::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CurrentHealth -= DamageAmount;
	if (CurrentHealth <= 0.0f)
	{
		//角色死亡
		UE_LOG(LogTemp, Warning, TEXT("I'm Die!"));
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = true;  //显示鼠标
			DisableInput(PlayerController);             //停止控制
		}
	}
	else {
		//角色未死亡，击退效果
		FVector KnockbackDirection = GetActorLocation() - DamageCauser->GetActorLocation();
		KnockbackDirection.Z = 0; // 保持水平击退
		KnockbackDirection.Normalize();
		LaunchCharacter(KnockbackDirection * 500.0f, true, true);
	}
	return DamageAmount;
}
