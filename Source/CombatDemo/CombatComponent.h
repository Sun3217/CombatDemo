// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class COMBATDEMO_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent();

	UPROPERTY(EditAnywhere,Category="Combat")
	float AttackRange = 140.0f;

	UPROPERTY(EditAnywhere,Category="Combat")
	float MaxHP = 60.0f;

	UPROPERTY(EditAnywhere,Category="Combat")
	TSubclassOf<class UCameraShakeBase>HitShakeClass;

	UPROPERTY(EditAnywhere,Category="UI")
	TSubclassOf<class UUserWidget>HealthWidgetClass;
	float HP;
	float ATK;

	UFUNCTION(BlueprintCallable)
	void LightAttack();

	AActor* CurrentEnemy;

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(float Damage);

	void RefreshHealthUI();

private:
	UPROPERTY()
	class UUserWidget* HealthWidget = nullptr;

	FTimerHandle RestartTimer;
	void RestartLevel();
protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
