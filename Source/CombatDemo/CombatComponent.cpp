// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatComponent.h"
#include "EnemyActor.h"
#include "Kismet/GameplayStatics.h"

UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	HP = 100.0f;
	ATK = 15.0f;
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("CombatComp BeginPlay 跑了！"));

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyActor::StaticClass(), FoundActors);
	UE_LOG(LogTemp, Warning, TEXT("找到 %d 个 EnemyActor"), FoundActors.Num());

	if (FoundActors.Num() > 0)
		CurrentEnemy = FoundActors[0];
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}
void UCombatComponent::LightAttack()
{
    UE_LOG(LogTemp, Warning, TEXT("LightAttack! ATK=%f"), ATK);

    // 每次攻击时实时找最近的敌人
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemyActor::StaticClass(), FoundActors);

    AEnemyActor* NearestEnemy = nullptr;
    float NearestDist = 999999.0f;

    for (AActor* A : FoundActors)
    {
        AEnemyActor* E = Cast<AEnemyActor>(A);
        if (E && E->HP > 0.0f)
        {
            float Dist = FVector::Dist(GetOwner()->GetActorLocation(), E->GetActorLocation());
            if (Dist < NearestDist)
            {
                NearestDist = Dist;
                NearestEnemy = E;
            }
        }
    }

    if (NearestEnemy && NearestDist <= 200.0f)
    {
        NearestEnemy->ApplyDamage(ATK);
    }
    else if (NearestEnemy)
    {
        UE_LOG(LogTemp, Warning, TEXT("太远了，距离=%f"), NearestDist);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("没有敌人"));
    }
}