// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Components/SkeletalMeshComponent.h"
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
	BeanBag,
};

USTRUCT(BlueprintType)
struct FProjectileInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AProjectile> Class;
	
	UPROPERTY(EditDefaultsOnly)
	int32 Cost;
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
	void RapidFire();

	UFUNCTION(BlueprintCallable, Category="Weapon")
	bool HitTrace(FHitResult& Hit, FVector& ShotDirection);

	UFUNCTION(BlueprintImplementableEvent)
	void FireTriggerEvent(const FHitResult &Hit, const FVector &ShotDirection, AMMO_TYPES AmmoType);

	UFUNCTION(BlueprintImplementableEvent)
	void ReloadTriggerEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void ReloadCompletedEvent();

	/** Reloading */
	UFUNCTION()
	void Reload();

	/** Equiped Ammo Type */
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<AMMO_TYPES> CurrentAmmoType = Regular;
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<AMMO_TYPES> CurrentAlternateAmmoType = Explosive;

	UPROPERTY(EditDefaultsOnly, Category="Ammo")
	TMap<TEnumAsByte<AMMO_TYPES>, FProjectileInfo> Projectiles = {
	   // { Name, { Class, Cost } },
		{ Regular,      { nullptr,	100 } },
		{ Bouncing,     { nullptr,	150 } },
		{ Rapid,        { nullptr,	50  } },
		{ Explosive,    { nullptr,	500 } },
		{ Flare,        { nullptr,	200 } },
		{ BeanBag,      { nullptr,	300 } },
	}; // Choose class in editor

	UFUNCTION()
	FProjectileInfo GetProjectileInfoByKey(AMMO_TYPES AmmoType) { return *Projectiles.Find(AmmoType); }
	
	UPROPERTY(BlueprintReadWrite)
	int32 CurrentMag = 1000;
	int32 MaxMagSize = 1000;

	UPROPERTY(BlueprintReadWrite)
	int32 AmmoCapacity = 10000;
	int32 MaxAmmoCapacity = 10000;

	UPROPERTY(BlueprintReadWrite)
	FTimerHandle ReloadTimerHandle;
	UPROPERTY(BlueprintReadOnly)
	float ReloadTime = 3.0;

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
	void SetPrimaryAmmoType(TEnumAsByte<AMMO_TYPES> AmmoType);
	void SetAlternateAmmoType(TEnumAsByte<AMMO_TYPES> AmmoType);

	bool bCanReload = true;

	void AttemptReload();

	/**Helper Functions */
	FRotator RandomRotator(float Pitch, float Yaw, float Roll, float Interval) const;
};