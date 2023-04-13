// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NavigationSystemTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Gun.generated.h"

UENUM()
enum AMMO_TYPES{Regular, Piercing, Explosive, Flare};

class AAcopalypsCharacter;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACOPALYPS_API UGun : public USkeletalMeshComponent
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

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputMappingContext* FireMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	class UInputAction* FireAction;
	
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

	/** Sets default values for this component's properties */
	UGun();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AttachWeapon(AAcopalypsCharacter* TargetCharacter);

	/** Make the weapon Fire */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void Fire();

	/** Make the weapon Alternate Fire */
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void AlternateFire();
protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	/** The Character holding this weapon*/
	AAcopalypsCharacter* Character;

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
	AController* GetOwnerController() const;

	/** Equiped Ammo Type */
	AMMO_TYPES CurrentAmmoType;
	AMMO_TYPES CurrentAlternateAmmoType;

	/** Ammo Setter Functions */
	void SetAmmoRegular();
	void SetAmmoExplosive();
	void SetAmmoFlare();
	void SetAmmoPiercing();

	/** Ammo Fire Functions */
	void FireRegular(FHitResult &Hit, FVector &ShotDirection);
	void FireExplosive(FHitResult &Hit, FVector &ShotDirection);
	void FireFlare(FHitResult &Hit, FVector &ShotDirection);
	void FirePiercing(FHitResult &Hit, FVector &ShotDirection);

	/** Reloading */
	UFUNCTION()
	void Reload();
};