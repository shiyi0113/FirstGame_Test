#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemySpawner.generated.h"

UCLASS()
class FIRSTGAME_MYTEST_API AEnemySpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AEnemySpawner();
	virtual void Tick(float DeltaTime) override;
	void SpawnMultipleRandomActors(int32 NumberOfActors);
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<AActor> ActorToSpawn;
};
