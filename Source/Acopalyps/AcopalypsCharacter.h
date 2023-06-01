// Copyright Epic Games, Inc. All Rights Reserved.
/**
 @author: Gin Lindelöw
*/

#pragma once

#include "CoreMinimal.h"
#include "Gun.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AcopalypsCharacter.generated.h"

class UAcopalypsSaveGame;
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
	FTimerHandle TimeTimerHandle;
	
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Mesh, meta = (AllowPrivateAccess = "true"))
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

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* CrouchActionSlowMotion;
	
	/** Slow Down Time Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SlowDownTimeAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* InteractAction;
	
	/** Respawn Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* RespawnAction;
	
	/** Reset Level Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ResetLevelAction;
	
	/** Save Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SaveAction;
	
	/** Load Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* LoadAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess=true))
	float MouseSensitivity = 0.6;

	public:
	AAcopalypsCharacter();

	UFUNCTION()
	void Respawn();

	UPROPERTY()
	FTimerHandle RespawnTimer;
	
	UPROPERTY()
	FTimerHandle PauseOverlapTimer;
	FTimerDelegate PauseOverlapDelegate;

	UFUNCTION()
	void ResetLevel();

	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Health)
	class UHealthComponent* HealthComponent;
	
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> AmmoStationClass;

	UFUNCTION(BlueprintImplementableEvent)
	void CrouchTriggerEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void SlideTriggerEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void TakeDamageTriggerEvent(const float& Damage, const AActor* DamageCauser);
	
	UFUNCTION(BlueprintImplementableEvent)
	void SlowTimeTriggerEvent();
	
	UFUNCTION(BlueprintImplementableEvent)
	void ResumeTimeTriggerEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void InteractTriggerEvent();
	
	UFUNCTION(BlueprintImplementableEvent)
	void SpawnTriggerEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void OnSave(const FString& SaveName);
	UFUNCTION(BlueprintImplementableEvent)
	void OnLoad();

	UFUNCTION(BlueprintImplementableEvent)
	void DeathEvent();
	
	/** Called upon when object channel weapon collider collides with enemy char */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintCallable)
	void Save();
	UFUNCTION(BlueprintCallable)
	void Load();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int NumberOfActiveCombats = 0;
	
protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	// Crouching functions
	void StartCrouch(bool SlowMotion);
	void EndCrouch();

	void StartSlide(bool SlowMotion);
	void EndSlide();
	
	virtual void Jump() override;

	void SlowDownTime();
	void ResumeTime();
	bool bTimeSlowed = false;
	
	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UAcopalypsSaveGame> SaveGameClass;

public:
	UPROPERTY(VisibleAnywhere)
	UAcopalypsSaveGame* SaveGame;
	
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
	// ---- GUN ---- //
	
	// Gun variables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Gun")
		TSubclassOf<AGun> GunClass;
	UPROPERTY(BlueprintReadOnly)
		AGun* Gun;
	
	// ---- MOVEMENT ---- //
	
	// Characters active movespeed variable
	UPROPERTY(VisibleAnywhere, Category="Movement")
		float WalkingMovementSpeed = 1000.f;

	// Crouching variables
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Movement|Crouch")
	bool bIsCrouching;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Movement|Crouch")
	bool bRequestStopCrouching = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement|Crouch")
	float CrouchMovementSpeed = 400.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement|Crouch")
	float CrouchSpeed = 6.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement|Crouch")
	float CrouchInterpTime;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Movement|Crouch")
	float CrouchHeight = 42.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Movement|Slide")
	bool bIsSliding;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Slide")
	float SlideStrength = 1100.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Movement|Slide")
	float SlideTime = .8f;
	UPROPERTY()
	FTimerHandle SlideHandle;
	
	UPROPERTY(VisibleAnywhere)
	bool bIsDead;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<struct FLevelID> LoadedLevels;

	UFUNCTION(BlueprintCallable)
	void SetLoadedLevels(TArray<FLevelID> LevelsToLoad);
};
