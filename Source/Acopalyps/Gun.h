// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Gun.generated.h"

UENUM()
enum AMMO_TYPES{Regular, Piercing, Explosive, Flare, Rapid};

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
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	UNiagaraSystem* MuzzleFlash;

	// Regular ammo impact sound and particle effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	UNiagaraSystem* ImpactEffectRegularAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	USoundBase* ImpactSoundRegularAmmo;
	
	// Piercing ammo impact sound and particle effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	UNiagaraSystem* ImpactEffectPiercingAmmo;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category=Gameplay)
	USoundBase* ImpactSoundPiercingAmmo;
	
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
	class UInputAction* ChangeAmmoPiercingAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* ChangeAmmoRapidAction;
	
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

	UFUNCTION(BlueprintImplementableEvent)
	void FireTriggerEvent(const FHitResult &Hit, const FVector &ShotDirection);

	void SetRegularMag(int32 Size);
	void SetPiercingMag(int32 Size);
	void SetExplosiveMag(int32 Size);
	void SetFlareMag(int32 Size);
	void SetRapidMag(int32 Size);

	/** Reloading */
	UFUNCTION()
	void Reload();
	/** Reloading */
	UFUNCTION()
	void AlternateReload();

	UFUNCTION(BlueprintGetter)
	AMMO_TYPES GetCurrentAmmoType();
	
	UFUNCTION(BlueprintGetter)
	AMMO_TYPES GetCurrentAlternateAmmoType();

	/** Equiped Ammo Type */
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<AMMO_TYPES> CurrentAmmoType;
	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<AMMO_TYPES> CurrentAlternateAmmoType;
	
	UPROPERTY(BlueprintReadOnly)
	int32 RegularMag = 12;
	UPROPERTY(BlueprintReadOnly)
	int32 PiercingMag = 12;
	UPROPERTY(BlueprintReadOnly)
	int32 ExplosiveMag = 1;
	UPROPERTY(BlueprintReadOnly)
	int32 FlareMag = 1;
	UPROPERTY(BlueprintReadOnly)
	int32 RapidMag = 12;

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
	float MaxRange = 5000.0;
	/** Damage Variable */
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float Damage = 10.f;
	
	/** Shot Impact */
	UPROPERTY(EditAnywhere, Category="Weapon Properties")
	float ImpulseForce = 500.f;

	/** Ammo variables **/
	UPROPERTY(EditDefaultsOnly)
	int32 MaxAmmo = 12;

	bool GunTrace(FHitResult &HitResult, FVector &ShootDirection);
	bool GunTraceInaccurate(FHitResult &HitResult, FVector &ShootDirection);
	AController* GetOwnerController() const;

	/** Ammo Setter Functions */
	void SetAmmoRegular();
	void SetAmmoExplosive();
	void SetAmmoFlare();
	void SetAmmoPiercing();
	void SetAmmoRapid();

	/** Ammo Fire Functions */
	void FireRegular(FHitResult &Hit, FVector &ShotDirection);
	void FireExplosive(FHitResult &Hit, FVector &ShotDirection);
	void FireFlare(FHitResult &Hit, FVector &ShotDirection);
	void FirePiercing(FHitResult &Hit, FVector &ShotDirection);
	void FireRapid(FHitResult &Hit, FVector &ShotDirection);

	/**Helper Functions */
	FRotator RandomRotator(float Pitch, float Yaw, float Roll, float Interval) const;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AExplosiveProjectile> ExplosiveProjectileClass;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class AProjectile> RegularProjectileClass;
	AExplosiveProjectile* ExplosiveProjectile;
};