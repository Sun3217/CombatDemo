// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyActor.generated.h"

enum EnemyState
{
	Idle,
	Chase,
	Attack,
	Dead
};

UCLASS()
class COMBATDEMO_API AEnemyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyActor();

	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* HealthBarWidget;

	float HP;
	float MaxHP;

	float AttackTimer;   // 攻击动画计时器

	EnemyState CurrentState;

	void ApplyDamage(float Damage);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
