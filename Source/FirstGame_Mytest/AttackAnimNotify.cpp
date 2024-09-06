#include "AttackAnimNotify.h"
#include "GamePlayer.h"
#include "GameEnemy.h"
void UAttackAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
    if (MeshComp && MeshComp->GetOwner())
    {
        AGamePlayer* Player = Cast<AGamePlayer>(MeshComp->GetOwner());
        AGameEnemy* Enemy = Cast<AGameEnemy>(MeshComp->GetOwner());
        if (Player)
        {
            Player->PerformAttack();
        }
        if (Enemy)
        {
            Enemy->PerformAttack();
        }
    }
}
