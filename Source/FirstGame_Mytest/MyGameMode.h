#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"
class AEnemySpawner;
UCLASS()
class FIRSTGAME_MYTEST_API AMyGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	AMyGameMode();
    virtual void BeginPlay() override;
    UFUNCTION()
	void OnEnemyDestroyed();
    void SpawnWave();
    int32 CurrentWave;       
    int32 EnemiesAlive;
    AEnemySpawner* EnemySpawner;
    TArray<int32> WaveEnemyCounts;
    FTimerHandle TimerHandle;
};
