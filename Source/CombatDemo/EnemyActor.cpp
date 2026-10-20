// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyActor.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CombatComponent.h"

// Sets default values
AEnemyActor::AEnemyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HP = 50.0f;
	MaxHP = 50.0f;
	AttackTimer = 0.0f;
	CurrentState = Idle;

	GetCapsuleComponent()->SetCapsuleSize(35.0f, 90.0f);

	GetCharacterMovement()->MaxWalkSpeed = 200.0f;
	
	bUseControllerRotationYaw = false;

	static ConstructorHelpers::FObjectFinder<USkeletalMesh> EnemyMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple"));
	if (EnemyMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(EnemyMesh.Object);
	}

	// 设置动画蓝图（让敌人有动画）
	static ConstructorHelpers::FClassFinder<UAnimInstance> EnemyAnimClass(TEXT("/Game/Variant_Combat/Anims/ABP_Manny_Combat"));
	if (EnemyAnimClass.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(EnemyAnimClass.Class);
	}
	GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));

	HealthBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(RootComponent);   // 挂根节点，不跟 Mesh 偏移
	HealthBarWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 110.0f));
	HealthBarWidget->SetWidgetSpace(EWidgetSpace::Screen);

	static ConstructorHelpers::FClassFinder<UUserWidget>WidgetClass(TEXT("/Game/Variant_Combat/UI/WB_EnemyHealthBar"));
	if (WidgetClass.Succeeded()) 
	{
		HealthBarWidget->SetWidgetClass(WidgetClass.Class);
	}
}

// Called when the game starts or when spawned
void AEnemyActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentState)
	{
	case Idle:
	{
		APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (Player) {
			float Distance = FVector::Dist2D(GetActorLocation(), Player->GetActorLocation());
			if (Distance < 500.0f) {
				CurrentState = Chase;
				UE_LOG(LogTemp, Warning, TEXT("Find Player!"));
			}
		}
		break;
	}
	case Chase: {
		APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (Player) {
			FVector MyLoc = GetActorLocation();
			FVector PlayerLoc = Player->GetActorLocation();
			float Distance = FVector::Dist2D(GetActorLocation(), Player->GetActorLocation());
			FVector Direction = (PlayerLoc - MyLoc).GetSafeNormal();

			// 面朝玩家
			if (!Direction.IsNearlyZero())
			{
				SetActorRotation(Direction.Rotation());
			}

			if (Distance > 150.0f) {
				AddMovementInput(Direction, 1.0f, /*bForce=*/true);
			}
			else {
				CurrentState = Attack;
				AttackTimer = 1.5f;
				UE_LOG(LogTemp, Warning, TEXT("Accact!"));
			}
		}
		break;
	}
	case Attack: {
		APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
		if (Player) {
			// 面朝玩家
			FVector Direction = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
			if (!Direction.IsNearlyZero())
			{
				SetActorRotation(Direction.Rotation());
			}

			float Distance = FVector::Dist2D(GetActorLocation(), Player->GetActorLocation());
			if (Distance > 220.0f) {
				CurrentState = Chase;
				AttackTimer = 0.0f;
				UE_LOG(LogTemp, Warning, TEXT("Player ran far,chasing after!"));
			}
			else {
				// 每隔 1.5 秒播放一次攻击动画
				AttackTimer += DeltaTime;
				if (AttackTimer >= 1.5f)
				{
					AttackTimer = 0.0f;
					if (UAnimInstance* EnemyAnim = GetMesh()->GetAnimInstance())
					{
						UAnimMontage* AttackMontage = LoadObject<UAnimMontage>(nullptr, TEXT("/Game/Variant_Combat/Anims/AM_ComboAttack"));
						if (AttackMontage)
						{
							EnemyAnim->Montage_Play(AttackMontage);
							GetWorld()->GetTimerManager().SetTimer(AttackHitTimer, this, &AEnemyActor::CheckAttackHit, 0.45f, false);
						}
					}
				}
			}
		}
		break;
	}

	case Dead: {
		break;
	}
	}
}

void AEnemyActor::ApplyDamage(float Damage)
{
	if (CurrentState == Dead)return;
	HP -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("Enemy 受到 %f 伤害！剩余 HP = %f"), Damage, HP);

	if (HealthBarWidget)
	{
		UUserWidget* Widget = HealthBarWidget->GetUserWidgetObject();
		if(Widget)
		{
			UProgressBar* Bar = Cast<UProgressBar>(Widget->GetWidgetFromName(TEXT("HealthBar")));
			if (Bar)
			{
				Bar->SetPercent(HP / MaxHP);
				UE_LOG(LogTemp, Warning, TEXT("Update health bar: %f"), HP / MaxHP);
			}
		}
	}

	if (HP <= 0.0f)
	{
		CurrentState = Dead;

		if (HealthBarWidget)HealthBarWidget->SetHiddenInGame(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->DisableMovement();
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);
		SetLifeSpan(5.0f);
	}
}

void AEnemyActor::CheckAttackHit()
{
	if (CurrentState == Dead)return;

	APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!Player)return;

	if (FVector::Dist2D(GetActorLocation(), Player->GetActorLocation()) > 200.0f)return;

	if (UCombatComponent* Comp = Player->FindComponentByClass<UCombatComponent>())
	{
		Comp->ApplyDamage(AttackDamage);
	}
}

