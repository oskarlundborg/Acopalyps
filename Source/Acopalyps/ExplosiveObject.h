// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveObject.generated.h"

UCLASS()
class ACOPALYPS_API AExplosiveObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveObject();

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class UHealthComponent* HealthComponent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	float ExplosionRadius = 360.0;
	
	/** Returns if object is dead*/
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnDeathTriggerEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void ExplosionOverlapActorEvent(const AActor* Actor);

	/** Called upon when object channel weapon collider collides with enemy char */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	bool ExplosionTrace(TArray<struct FOverlapResult>& Overlaps) const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UExplosiveDamageType> ExplosiveDamageType;

};
