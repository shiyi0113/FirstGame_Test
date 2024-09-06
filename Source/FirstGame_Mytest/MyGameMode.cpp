#include "MyGameMode.h"
#include "EnemySpawner.h"
#include "Kismet/GameplayStatics.h"
AMyGameMode::AMyGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Blueprints/BP_GamePlayer"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	CurrentWave = 0;
	EnemiesAlive = 0;
	WaveEnemyCounts = { 5, 10, 20 };// 初始化每一波敌人的数量
	TArray<AActor*> FoundSpawners;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemySpawner::StaticClass(), FoundSpawners);

	if (FoundSpawners.Num() > 0)
	{
		EnemySpawner = Cast<AEnemySpawner>(FoundSpawners[0]);
	}
}

void AMyGameMode::BeginPlay()
{
	Super::BeginPlay();
	// 生成第一波敌人
	SpawnWave();
}

void AMyGameMode::SpawnWave()
{
	if (EnemySpawner && WaveEnemyCounts.IsValidIndex(CurrentWave))
	{
		int32 NumberOfEnemies = WaveEnemyCounts[CurrentWave];
		EnemySpawner->SpawnMultipleRandomActors(NumberOfEnemies);
		UE_LOG(LogTemp, Warning, TEXT("%d"), NumberOfEnemies);
		EnemiesAlive = NumberOfEnemies;
	}
}

void AMyGameMode::OnEnemyDestroyed()
{
	UE_LOG(LogTemp, Warning, TEXT("in"));
	EnemiesAlive--;
	if (EnemiesAlive <= 0)
	{
		CurrentWave++;
		if (WaveEnemyCounts.IsValidIndex(CurrentWave))
		{
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AMyGameMode::SpawnWave, 10.0f, false);
		}
		else
		{
			// 所有波次完成后的逻辑
			UE_LOG(LogTemp, Warning, TEXT("All waves completed!"));
		}
	}
}