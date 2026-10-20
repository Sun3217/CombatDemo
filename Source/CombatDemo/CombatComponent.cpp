// Fill out your copyright notice in the Description page of Project Settings.

#include "CombatComponent.h"
#include "EnemyActor.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraShakeBase.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "CombatDemoCharacter.h"
#include "GameFramework/SpringArmComponent.h"

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

    HP = MaxHP;

    if (HealthWidgetClass)
    {
        HealthWidget = CreateWidget<UUserWidget>(GetWorld(), HealthWidgetClass);
            if (HealthWidget)
            {
                HealthWidget->AddToViewport();
                RefreshHealthUI();
            }
    }
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

    const FVector OwnerLoc = GetOwner()->GetActorLocation();
    const FVector Forward = GetOwner()->GetActorForwardVector().GetSafeNormal2D();

    for (AActor* A : FoundActors)
    {
        AEnemyActor* E = Cast<AEnemyActor>(A);
        if (!E || E->HP <= 0.0f)
        {
            continue;
        }

        const FVector ToEnemy = (E->GetActorLocation() - OwnerLoc).GetSafeNormal2D();
        if (FVector::DotProduct(Forward, ToEnemy) <= 0.5f)
        {
            continue;
        }

        const float Dist = FVector::Dist2D(OwnerLoc, E->GetActorLocation());
        if (Dist < NearestDist)
        {
            NearestDist = Dist;
            NearestEnemy = E;
        }
    }

    if (NearestEnemy && NearestDist <= AttackRange)
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

void UCombatComponent::ApplyDamage(float Damage)
{
    if (HP <= 0.0f || Damage <= 0.0f)return;

    HP = FMath::Max(0.0f, HP - Damage);
    UE_LOG(LogTemp, Warning, TEXT("Player takes %f damage,HP=%f"), Damage, HP);
    RefreshHealthUI();

    if (HitShakeClass)
    {
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            if (APlayerCameraManager* CamMgr = PC->PlayerCameraManager)
            {
                CamMgr->StartCameraShake(HitShakeClass);
            }
        }
    }

    if (HP <= 0.0f)
    {
        if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
        {
            PC->DisableInput(PC);
            PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, 3.0f, FColor::Black, false, true);
        }

        if (ACharacter* Char = Cast<ACharacter>(GetOwner()))
        {
            Char->GetCharacterMovement()->DisableMovement();
            Char->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

            Char->GetMesh()->SetAnimInstanceClass(nullptr);

            Char->GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
            Char->GetMesh()->SetSimulatePhysics(true);
        }
        
        GetWorld()->GetTimerManager().SetTimer(RestartTimer, this, &UCombatComponent::RestartLevel, 3.5f, false);
    }
}

void UCombatComponent::RefreshHealthUI()
{
    if (!HealthWidget)return;

    if (UProgressBar* Bar = Cast<UProgressBar>(HealthWidget->GetWidgetFromName(TEXT("HealthBar"))))
    {
        Bar->SetPercent(HP / MaxHP);
    }
}

void UCombatComponent::RestartLevel()
{
    const FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
    UGameplayStatics::OpenLevel(GetWorld(), FName(*LevelName));
}