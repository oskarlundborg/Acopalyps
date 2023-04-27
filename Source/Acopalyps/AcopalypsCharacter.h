// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AcopalypsCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class AAcopalypsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Timer handle for all timers*/
	FTimerHandle TimerHandle;
	FTimerHandle TimeTimerHandle;
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** Leg mesh: visible only when kicking */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* LegMesh;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCharacterMovementComponent* CharacterMovementComponent;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SprintAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;
	
	/** Slow Down Time Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SlowDownTimeAction;
	
	/** Kick Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* KickAction;

	/** Kick Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* InteractAction;
	
	/** Kick force to add on other object on kick-hitbox-overlap*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Attack, meta=(AllowPrivateAccess = "true"))
	FVector KickForce = FVector(0, 0, 5000);
	//TODO inför framtiden kanske: Ha collisionsboxes som sätts aktiva onAnimNotifyState - när benanimationen är i ett visst läge, då sätts colliders till "mottagliga" för coll
	// TODO inför framtiden kanske: Ha kick som ett enum, ett attackEnum för enklare uppbyggnad? Om vi vill kunna slå sönder saker?
	
	FHitResult LookHit;
	
	public:
	AAcopalypsCharacter();

	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay();

	UPROPERTY(EditDefaultsOnly)
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere)
	float Health;

public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
	bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
	bool GetHasRifle();

	/** Triggered on collision hit event between leg hitbox and enemies*/
	UFUNCTION()
	void OnKickAttackHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnKickAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UFUNCTION(BlueprintImplementableEvent)
	void KickTriggerEvent(const FHitResult &Hit);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SprintTriggerEvent();
	
	UFUNCTION(BlueprintImplementableEvent)
	void CrouchTriggerEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void SlideTriggerEvent(const FVector& Velocity);

	UFUNCTION(BlueprintImplementableEvent)
	void TakeDamageTriggerEvent(const float& Damage, const AActor* DamageCauser);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SlowTimeTriggerEvent();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ResumeTimeTriggerEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void InteractTriggerEvent();
	
	/** Called upon when object channel weapon collider collides with enemy char */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	/** Returns if charachter is dead*/
	UFUNCTION(BlueprintPure)
	bool IsDead() const;

	UFUNCTION(BlueprintPure)
	float GetHealthPercent() const;

	
protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	// Crouching funtions
	void StartCrouch();
	void EndCrouch();
	
	// Crouching funtions
	void StartSprint();
	void EndSprint();

	virtual void Jump() override;
	//void StopJumping() override;

	void SlowDownTime();
	void ResumeTime();
	
	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for kicking input*/
	void Kick();

	/** Called after kicking timer ended*/
	void HideLeg() const;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	/** Returns AmmoCountMap TMap<AMMO_TYPES, int32> **/
	TMap<TEnumAsByte<AMMO_TYPES>, int32>* GetAmmoCountMap() { return &AmmoCountMap; }


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gun")
	TSubclassOf<class AActor> AmmoStationClass;
	
	// ---- GUN ---- //
	
	// Gun variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gun")
		TSubclassOf<class AGun> GunClass;
	UPROPERTY(BlueprintReadOnly)
		AGun* Gun;

	// Map of Ammo types and their current amount
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gun|Ammo")
		TMap<TEnumAsByte<AMMO_TYPES>, int32> AmmoCountMap = {
			// Left Barrel
			{ Regular,   100 },
			{ Bouncing,  100 },
			{ Rapid,	  300 },
			// Right Barrel
			{ Explosive, 100 },
			{ Flare,     100 },
			{ BeanBag,	  10 },
		};

	UFUNCTION(BlueprintCallable)
	void RefillAllAmmo()
	{
		UE_LOG(LogTemp,Display,TEXT("ME WANT GIVE AMMO"))
		AmmoCountMap = {
			// Left Barrel
			{ Regular,   100 },
			{ Bouncing,  100 },
			{ Rapid,	  300 },
			// Right Barrel
			{ Explosive, 100 },
			{ Flare,     100 },
			{ BeanBag,	  10 },
		};
		Health = MaxHealth;
	}

	UFUNCTION(BlueprintCallable)
	void RefillAmmo(AMMO_TYPES AmmoType, int32 Amount)
	{
		AmmoCountMap.Emplace(AmmoType, Amount);
	}
	
	// ---- MOVEMENT ---- //
	
	// Characters active movespeed variable
	UPROPERTY(VisibleAnywhere, Category="Movement|Crouch")
		float WalkingMovementSpeed = 600.f;

	// Crouching variables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Movement|Crouch")
		bool bIsCrouching;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement|Crouch")
		float CrouchMovementSpeed = 300.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement|Crouch")
		float CrouchSpeed = 5.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement|Crouch")
		float CrouchInterpTime;

	// Sprinting variables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Movement|Sprint")
		bool bIsSprinting;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement|Sprint")
		float SprintMovementSpeed = 1000.f;
};

