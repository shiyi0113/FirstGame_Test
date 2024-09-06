#include "GameEnemy.h"
#include "GamePlayer.h"
#include "Engine/DamageEvents.h"
#include "Components/ArrowComponent.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/ProgressBar.h"
#include "AIController.h"
#include "MyGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AGameEnemy::AGameEnemy()
{
	PrimaryActorTick.bCanEverTick = true;
	AttackArrow = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	AttackArrow->SetupAttachment(RootComponent);
	AttackBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackBox"));
	AttackBox->SetupAttachment(RootComponent);
	HealthBarComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarComponent"));
	HealthBarComponent->SetupAttachment(RootComponent);

	GetCharacterMovement()->bOrientRotationToMovement = true;   //将旋转朝向运动
	GetCharacterMovement()->MaxWalkSpeed = 250.f;               //最大移动速度
	MaxHealth = 20.0f;
	CurrentHealth = MaxHealth;
	CanAttack = true;
	EnemyIsDie = false;
	Damage = 10.0f;
}

void AGameEnemy::BeginPlay()
{
	Super::BeginPlay();
	TargetActor = Cast<AGamePlayer>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	MyGameMode = Cast<AMyGameMode>(UGameplayStatics::GetGameMode(this));
	UpdateHealthBar();
}

void AGameEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(!EnemyIsDie)
		MoveToTarget();
}
//受伤
float AGameEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	CurrentHealth -= DamageAmount;
	UpdateHealthBar();
	if (CurrentHealth <= 0.0f)
	{
		//死亡
		EnemyIsDie = true;
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		SetLifeSpan(2.5f);
		if (MyGameMode)
		{
			MyGameMode->OnEnemyDestroyed();
		}
	}
	else {
		//未死亡
		FVector KnockbackDirection = GetActorLocation() - DamageCauser->GetActorLocation();
		KnockbackDirection.Z = 0; // 保持水平击退
		KnockbackDirection.Normalize();
		LaunchCharacter(KnockbackDirection * 500.0f, true, true);
	}
	return DamageAmount;
}
//追踪角色
void AGameEnemy::MoveToTarget()
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (TargetActor) 
		AIController->MoveToActor(TargetActor, 5.0f, true, true, true, 0, true);
}
//攻击动画
void AGameEnemy::Attack()
{
	if (CanAttack && AttackMontage && CurrentHealth > 0 && TargetActor->CurrentHealth > 0) {
		PlayAnimMontage(AttackMontage);
		CanAttack = false;
		FOnMontageEnded MontageEndedDelegate;   //创建一个委托
		MontageEndedDelegate.BindUObject(this, &AGameEnemy::OnAttackMontageEnded);  //将委托绑定到这个函数OnAttackMontageEnded
		GetMesh()->GetAnimInstance()->Montage_SetEndDelegate(MontageEndedDelegate, AttackMontage); //蒙太奇动画播放完后执行函数
	}
}

void AGameEnemy::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (Montage == AttackMontage) {
		CanAttack = true;
	}
}
//出伤   (通过动画通知调用)
void AGameEnemy::PerformAttack()
{
	FVector Start = AttackArrow->GetComponentLocation();
	FVector End = Start + AttackArrow->GetForwardVector() * 50.0f;
	TArray<FHitResult> HitResults;
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(30.0f);
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
			AGamePlayer* HitActor = Cast<AGamePlayer>(Hit.GetActor());
			if (HitActor == TargetActor) {
				FDamageEvent DamageEvent;
				HitActor->TakeDamage(Damage, DamageEvent, GetController(), this);
				break;
			}
		}
	}
}
//血条UI
void AGameEnemy::UpdateHealthBar()
{
	if (HealthBarComponent)
	{
		UUserWidget* Widget = HealthBarComponent->GetUserWidgetObject();
		if (Widget)
		{
			UProgressBar* HealthProgressBar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("HealthProgressBar")));
			if (HealthProgressBar)
			{
				HealthProgressBar->SetPercent(CurrentHealth / MaxHealth);
			}
		}
	}
}
