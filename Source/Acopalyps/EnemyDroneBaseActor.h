// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EnemyDroneBaseActor.generated.h"

UCLASS()
class ACOPALYPS_API AEnemyDroneBaseActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEnemyDroneBaseActor();

	UPROPERTY(VisibleAnywhere, Category=Health)
	class UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, Category=Collision)
	class USphereComponent* SphereColliderComponent;

	UPROPERTY(EditAnywhere, Category=Collision)
	UStaticMeshComponent* DroneMesh;

	UFUNCTION()
	void OnAttackOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnAttackOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	/** Returns if charachter is dead
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;
	*/
	UPROPERTY(EditAnywhere)
	float Speed;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/*
	UPROPERTY(EditAnywhere, Category = "Movement")
	UCurveFloat* MovementCurve;

	UFUNCTION()
	void OnTimelineUpdate(float Value);

	FOnTimeLineFloat* InterpFunction{};

	*/
private:
	UPROPERTY(EditAnywhere)
	class AAcopalypsCharacter* PlayerCharacter;

	FVector EngageTargetLocation;

	FVector AttackTargetLocation;

	FVector PlayerLocation;
	float PlayerHeight;

	FVector Direction;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	float DistanceToPlayer = 100.f; // Set the desired distance from the player here

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess = true))
	double MoveToEngagedTargetDelay = 0.5f;
	//double TimeSinceLastMove = 0.f;

	FTimerHandle MoveTimer;
	void MoveTowardsEngagedLocation();

	UPROPERTY(EditAnywhere)
	class UTimelineComponent* TimeLine;

};
