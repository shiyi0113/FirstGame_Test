#include "EnemySpawner.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemySpawner::SpawnMultipleRandomActors(int32 NumberOfActors)
{
	for (int32 i = 0; i < NumberOfActors; ++i)
	{
		FVector RandomLocation = GetActorLocation() + FVector(FMath::FRandRange(-3000.0f, 3000.0f), FMath::FRandRange(-3000.0f, 3000.0f), 0.0f); // 随机位置，半径500单位
		FRotator RandomRotation = FRotator::ZeroRotator; // 固定旋转角度，也可以随机化
		if (ActorToSpawn)
		{
			AActor* SpawnedActor = GetWorld()->SpawnActor<AActor>(ActorToSpawn, RandomLocation, RandomRotation);
			if (SpawnedActor)
				continue;
			else
				i--;
		}
	}
}
