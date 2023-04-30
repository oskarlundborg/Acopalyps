// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Gun.generated.h"

UENUM()
enum AMMO_TYPES
{
	Regular,
	Bouncing,
	Explosive,
	Flare,
	Rapid,
	BeanBag,
};

class AAcopalypsCharacter;
class UNiagaraSystem;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACOPALYPS_API AGun : public AActor
{
	GENERATED_BODY()
public:
	
	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector MuzzleOffset;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	UNiagaraSystem* MuzzleFlash;

	// Regular ammo impact sound and particle effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	UNiagaraSystem* ImpactEffectRegularAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	USoundBase* ImpactSoundRegularAmmo;
	
	// Piercing ammo impact sound and particle effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	UNiagaraSystem* ImpactEffectBouncingAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	USoundBase* ImpactSoundBouncingAmmo;
	
	// Explosive ammo impact sound and particle effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	UNiagaraSystem* ImpactEffectExplosiveAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	USoundBase* ImpactSoundExplosiveAmmo;
	
	// Flare ammo impact sound and particle effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	UNiagaraSystem* ImpactEffectFlareAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	USoundBase* ImpactSoundFlareAmmo;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* RapidFireAction;
	
	/** Alternate Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* AlternativeFireAction;
	
	/** Relaod Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ReloadAction;
	
	/** Change Ammo to Regular Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ChangeAmmoRegularAction;

	/** Change Ammo to Explosive Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ChangeAmmoExplosiveAction;

	/** Change Ammo to Flare Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ChangeAmmoFlareAction;

	/** Change Ammo to Piercing Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ChangeAmmoBouncingAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ChangeAmmoRapidAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ChangeAmmoBeanBagAction;
	
	/** Sets default values for this component's properties */
	AGun();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
		void AttachWeaponInputs(AAcopalypsCharacter* TargetCharacter);

	/** Make the weapon Fire */
	UFUNCTION(BlueprintCallable, Category="Weapon")
		void Fire();

	//Make the weapon fire RAPIDLY*/
	UFUNCTION(BlueprintCallable, Category="Weapon")
		void RapidFire();

	/** Make the weapon Alternate Fire */
	UFUNCTION(BlueprintCallable, Category="Weapon")
		void AlternateFire();

	UFUNCTION(BlueprintCallable, Category="Weapon")
		bool GunTrace(FHitResult& Hit, FVector& ShotDirection);

	UFUNCTION(BlueprintImplementableEvent)
		void FireTriggerEvent(const FHitResult &Hit, const FVector &ShotDirection, AMMO_TYPES AmmoType);

	UFUNCTION(BlueprintImplementableEvent)
		void ReloadTriggerEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void ReloadCompletedEvent();

	/** Reloading */
	UFUNCTION()
	void Reload();

	UFUNCTION(BlueprintGetter)
	AMMO_TYPES GetCurrentAmmoType();
	
	UFUNCTION(BlueprintGetter)
	AMMO_TYPES GetCurrentAlternateAmmoType();

	/** Equiped Ammo Type */
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<AMMO_TYPES> CurrentAmmoType;
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<AMMO_TYPES> CurrentAlternateAmmoType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammo")
	TMap<TEnumAsByte<AMMO_TYPES>, int32> AmmoTypeCostValues = {
		{ Regular, 100},
		{Bouncing, 150},
		{Rapid, 50},
		{Explosive, 500},
		{Flare, 200},
		{BeanBag, 300},
	};
	
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentMag = 1000;

	int32 MaxMagSize = 1000;

	UPROPERTY(BlueprintReadOnly)
	int32 AmmoCapacity = 10000;

	int32 MaxAmmoCapacity = 10000;

	UPROPERTY(BlueprintReadOnly)
	FTimerHandle ReloadTimerHandle;
	UPROPERTY(BlueprintReadOnly)
	float ReloadTime = 3.0;

	UFUNCTION(BlueprintCallable)
	void RefillAllAmmo()
	{
		AmmoCapacity = MaxAmmoCapacity;
	}

protected:
	UFUNCTION()
	virtual void BeginPlay() override;
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The Character holding this weapon*/
	AAcopalypsCharacter* Character;

	

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float InaccuracyModifier = 8.0;

	/** Weapon Max Range */
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float MaxRange = 10000.0;
	/** Damage Variable */
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float Damage = 10.f;
	
	/** Shot Impact */
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float ImpulseForce = 500.f;

	/** Ammo variables **/
	UPROPERTY(EditDefaultsOnly)
	int32 MaxAmmo = 12;
	
	AController* GetOwnerController() const;

	/** Ammo Setter Functions */
	void SetAmmoRegular();
	void SetAmmoExplosive();
	void SetAmmoFlare();
	void SetAmmoBouncing();
	void SetAmmoRapid();
	void SetAmmoBeanBag();

	/** Ammo Fire Functions */
	void FireRegular();
	void FireExplosive();
	void FireFlare();
	void FireBouncing();
	void FireRapid();
	void FireBeanBag();
public:
	void FireEnemy();
private:

	bool bCanReload = true;

	void AttemptReload();

	/**Helper Functions */
	FRotator RandomRotator(float Pitch, float Yaw, float Roll, float Interval) const;
	
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class AProjectile> RegularProjectileClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class AExplosiveProjectile> ExplosiveProjectileClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class AFlareProjectile> FlareProjectileClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class ABouncingProjectile> BouncingProjectileClass;
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class ABeanBagProjectile> BeanBagProjectileClass;
};