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
	//��ȡ��ɫ�Ľ���������������ô�С
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	//�ý�ɫ������������ĸ����ǡ�ƫ�ƽǡ�����
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	//��ȡ��ɫ�Ľ�ɫ�ƶ����
	GetCharacterMovement()->bOrientRotationToMovement = true;   //����ת�����˶�
	GetCharacterMovement()->MaxWalkSpeed = 500.f;               //����ƶ��ٶ�
	//���ɱ����
	CameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	CameraArm->SetupAttachment(RootComponent);
	CameraArm->TargetArmLength = 400.0f; 	                    //Ŀ��۳���
	CameraArm->bUsePawnControlRotation = true;                  //ʹ��Pawn������ת
	//�����
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraArm, USpringArmComponent::SocketName);  //�󵽵��ɱ�ĩ��
	FollowCamera->bUsePawnControlRotation = false;             //�ر�ʹ��Pawn������ת����Ϊ���ɱۿ�����
	//��ͷ���
	AttackArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	AttackArrow->SetupAttachment(RootComponent);
	//��ʼ��һЩ����
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
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);   //ת������ǿ����

	EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AGamePlayer::Jump);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGamePlayer::Move);
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGamePlayer::Look);
	EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AGamePlayer::Attack);
}
//�ƶ�
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
//������ӽ�
void AGamePlayer::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}
//��Ծ
void AGamePlayer::Jump()
{
	bPressedJump = true;
}
//���Ź�������
void AGamePlayer::Attack()
{
	if (CanAttack && AttackMontage) {
		PlayAnimMontage(AttackMontage);
		CanAttack = false;
		FOnMontageEnded MontageEndedDelegate;   //����һ��ί��
		MontageEndedDelegate.BindUObject(this, &AGamePlayer::OnAttackMontageEnded);  //��ί�а󶨵��������OnAttackMontageEnded
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndedDelegate, AttackMontage); //��̫�涯���������ִ�к���
	}
}

void AGamePlayer::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage) {
		CanAttack = true;
	}
}
//�������˼��  ��ʹ�ö���֪ͨ���ã�
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
		//��ɫ����
		UE_LOG(LogTemp, Warning, TEXT("I'm Die!"));
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		
		APlayerController* PlayerController = Cast<APlayerController>(GetController());
		if (PlayerController)
		{
			PlayerController->bShowMouseCursor = true;  //��ʾ���
			DisableInput(PlayerController);             //ֹͣ����
		}
	}
	else {
		//��ɫδ����������Ч��
		FVector KnockbackDirection = GetActorLocation() - DamageCauser->GetActorLocation();
		KnockbackDirection.Z = 0; // ����ˮƽ����
		KnockbackDirection.Normalize();
		LaunchCharacter(KnockbackDirection * 500.0f, true, true);
	}
	return DamageAmount;
}
