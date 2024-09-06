#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyGameMode.generated.h"
class AEnemySpawner;
class USoundBase;
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
    UPROPERTY(BlueprintReadOnly)
    int32 CurrentWave;  
    UPROPERTY(BlueprintReadOnly)
    int32 EnemiesAlive;
    AEnemySpawner* EnemySpawner;
    TArray<int32> WaveEnemyCounts;
    FTimerHandle TimerHandle;
    USoundBase* WaveAudio;
};
