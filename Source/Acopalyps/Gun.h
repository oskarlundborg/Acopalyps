// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Gun.generated.h"

class AAcopalypsCharacter;
class UNiagaraSystem;
class AProjectile;
class AExplosiveProjectile;
class AFlareProjectile;
class ABouncingProjectile; 
class ABeanBagProjectile;

UENUM(BlueprintType)
enum AMMO_TYPES
{
	Regular,
	Bouncing,
	Explosive,
	Flare,
	Rapid,
	Shotgun,
};

USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AProjectile> Class;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 Cost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float Delay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 ProjectilesToFire;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BurstDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ChargeDelay;
	
	FTimerHandle TimerHandle = FTimerHandle();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bCanFire = true;
};

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FRotator FlashlightRotation;

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
	const class UInputAction* ChangeAmmoRegularAction;

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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* RotateRegularAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* RotateAlternateAmmo;

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeaponInputs(ACharacter* TargetCharacter);
	
	/** Make the weapon Fire */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire(TEnumAsByte<AMMO_TYPES> AmmoType);

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void PrimaryFire();
	
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AlternateFire();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	void RapidFire() { if( CurrentAmmoType == Rapid ) Fire(Rapid); }

	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool HitTrace(FHitResult& Hit, FVector& ShotDirection);

	UFUNCTION(BlueprintImplementableEvent)
	void FireTriggerEvent(const FHitResult &Hit, const FVector &ShotDirection, AMMO_TYPES AmmoType);

	UFUNCTION(BlueprintImplementableEvent)
	void ReloadTriggerEvent();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnCanFire(AMMO_TYPES AmmoType);

	UFUNCTION(BlueprintImplementableEvent)
	void ReloadCompletedEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void EmptyMagFireAttemptEvent();

	/** Reloading */
	UFUNCTION()
	void Reload();

	UPROPERTY()
	FTimerHandle BurstTimerHandle;
	
	TArray<AMMO_TYPES> AmmoTypes = {Regular, Rapid, Bouncing};
	TArray<AMMO_TYPES> AlternateAmmoTypes = {Explosive, Shotgun, Flare};
	/** Equiped Ammo Type */
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<AMMO_TYPES> CurrentAmmoType = Regular;
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<AMMO_TYPES> CurrentAlternateAmmoType = Explosive;

	UPROPERTY(EditDefaultsOnly, Category="Ammo")
	TMap<TEnumAsByte<AMMO_TYPES>, FProjectileInfo> Projectiles = {
	   // { Name, { Class, Cost, Delay, ProjectilesToFire, BurstDelay, ChargeDelay } },
		{ Regular,   { nullptr,	100, .3f,   1, 0,   0   } },
		{ Bouncing,  { nullptr,	150, 0.1f,  3, .6f, 1   } },
		{ Rapid,     { nullptr,	50,  0.f,   1, 0,   0   } },
		{ Explosive, { nullptr,	0,   2.5f,  1, 0,   0   } },
		{ Flare,     { nullptr,	200, 2.f,   1, 0,   0   } },
		{ Shotgun,   { nullptr,	300, 0.8f,  8, 0,   0   } },
	}; // Choose class in editor

	UFUNCTION(BlueprintCallable)
	FProjectileInfo GetProjectileInfoByKey(AMMO_TYPES AmmoType) { return *Projectiles.Find(AmmoType); }
	
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentMag = 1000;
	int32 MaxMagSize = 1000;

	UPROPERTY(BlueprintReadWrite)
	int32 AmmoCapacity = 2147483647;
	int32 MaxAmmoCapacity = 2147483647;

	UPROPERTY(BlueprintReadWrite)
	FTimerHandle ReloadTimerHandle;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Weapon Properties")
	float ReloadTime = 1.5;
	
	UFUNCTION(BlueprintCallable, Category=Delay)
	void ToggleCanFirePrimary(AMMO_TYPES AmmoType)
	{
		Projectiles.Find(AmmoType)->bCanFire ^= true;
		if( Projectiles.Find(AmmoType)->bCanFire == true ) OnCanFire(AmmoType);
	}
	FTimerDelegate CanFirePrimaryDelegate;
	
	UFUNCTION(BlueprintCallable, Category=Delay)
	void ToggleCanFireAlternate(AMMO_TYPES AmmoType)
	{
		Projectiles.Find(AmmoType)->bCanFire ^= true;
		if( Projectiles.Find(AmmoType)->bCanFire == true ) OnCanFire(AmmoType);
	}
	FTimerDelegate CanFireAlternateDelegate;

	UFUNCTION(BlueprintCallable)
	void RefillAllAmmo() { AmmoCapacity = MaxAmmoCapacity; }

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The Character holding this weapon*/
	UPROPERTY(VisibleAnywhere)
	ACharacter* Character;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float InaccuracyModifier = 5.0;

	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float ShotgunSpread = 2.0;

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
	void SetPrimaryAmmoType(TEnumAsByte<AMMO_TYPES> AmmoType);
	void SetAlternateAmmoType(TEnumAsByte<AMMO_TYPES> AmmoType);

	void RotateAmmoType();
	void RotateAlternateAmmoType();

	bool bCanReload = true;

	void AttemptReload();

	/**Helper Functions */
	FRotator RandomRotator(float Pitch, float Yaw, float Roll, float Interval) const;
};