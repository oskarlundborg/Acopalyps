// Fill out your copyright notice in the Description page of Project Settings.


#include "Gun.h"
#include "AcopalypsCharacter.h"
#include "AcopalypsProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/DamageEvents.h"

UGun::UGun()
{
	MuzzleOffset = FVector(100.0, 0.0, 10.0);
	SetAmmoRegular();
}

void UGun::Fire()
{
	
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}
	
	FHitResult Hit;
	FVector ShotDirection;

	switch (CurrentAmmoType)
	{
	case Regular:
		FireRegular(Hit, ShotDirection);
		break;
	case Explosive:
		FireExplosive(Hit, ShotDirection);
		break;
	case Flare:
		FireFlare(Hit, ShotDirection);
		break;
	case Piercing:
		FirePiercing(Hit, ShotDirection);
		break;
	}
	AController* OwnerController = GetOwnerController();
	if(OwnerController == nullptr)
	{
		
		return;
	}
	UStaticMeshComponent* MeshComponent  = Cast<UStaticMeshComponent>(Hit.GetActor()->GetRootComponent());
	if(MeshComponent && Hit.GetActor()->IsRootComponentMovable())
	{
		FVector Location;
		FRotator Rotation;
		OwnerController->GetPlayerViewPoint(Location, Rotation);
		ShotDirection = Rotation.Vector();
		MeshComponent->AddImpulse(ShotDirection * ImpulseForce * MeshComponent->GetMass());
	}
}

void UGun::AttachWeapon(AAcopalypsCharacter* TargetCharacter)
{
	Character = TargetCharacter;
	if (Character == nullptr)
	{
		return;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
	
	// switch bHasRifle so the animation blueprint can switch to another animation set
	Character->SetHasRifle(true);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{

		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}
		
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UGun::Fire);
			EnhancedInputComponent->BindAction(ChangeAmmoRegularAction, ETriggerEvent::Triggered, this, &UGun::SetAmmoRegular);
			EnhancedInputComponent->BindAction(ChangeAmmoExplosiveAction, ETriggerEvent::Triggered, this, &UGun::SetAmmoExplosive);
			EnhancedInputComponent->BindAction(ChangeAmmoFlareAction, ETriggerEvent::Triggered, this, &UGun::SetAmmoFlare);
			EnhancedInputComponent->BindAction(ChangeAmmoPiercingAction, ETriggerEvent::Triggered, this, &UGun::SetAmmoPiercing);
		}
	}
}

bool UGun::GunTrace(FHitResult& HitResult, FVector& ShotDirection)
{
	
	AController* OwnerController = GetOwnerController();
	if(OwnerController == nullptr)
	{
		
		return false;
	}
	
	FVector Location;
	FRotator Rotation;
	OwnerController->GetPlayerViewPoint(Location, Rotation);
	ShotDirection = -Rotation.Vector();
	FVector End = Location + Rotation.Vector() * MaxRange;
	
	FCollisionQueryParams Params;
	Params.AddIgnoredComponent(this);
	Params.AddIgnoredActor(Character);
	return GetWorld()->LineTraceSingleByChannel(HitResult,Location,End,ECollisionChannel::ECC_GameTraceChannel1, Params);
}

AController* UGun::GetOwnerController() const
{
	AActor* Owner=GetOwner();
	APawn* OwnerPawn = Cast<APawn>(Character);
	if(OwnerPawn == nullptr)
	{
		return nullptr;
	}
	return OwnerPawn->GetController();
}


void UGun::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character == nullptr)
	{
		return;
	}

	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}

void UGun::SetAmmoRegular()
{
	CurrentAmmoType=AMMO_TYPES::Regular;
}

void UGun::SetAmmoExplosive()
{
	CurrentAmmoType=AMMO_TYPES::Explosive;
}

void UGun::SetAmmoFlare()
{
	CurrentAmmoType=AMMO_TYPES::Flare;
}

void UGun::SetAmmoPiercing()
{
	CurrentAmmoType=AMMO_TYPES::Piercing;
}

void UGun::FireRegular(FHitResult& Hit, FVector& ShotDirection)
{
	if(GunTrace(Hit, ShotDirection))
	{
		
		DrawDebugSphere(GetWorld(),Hit.Location,10,10,FColor::Cyan,true,5);
		AActor* HitActor = Hit.GetActor();
		if(HitActor != nullptr)
		{
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, GetOwnerController(), GetOwner());;
		}
	}
	
	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}


void UGun::FireExplosive(FHitResult& Hit, FVector& ShotDirection)
{
	if(GunTrace(Hit, ShotDirection))
	{
		
		DrawDebugSphere(GetWorld(),Hit.Location,30,10,FColor::Red,true,5);
		
		AActor* HitActor = Hit.GetActor();
		if(HitActor != nullptr)
		{
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, GetOwnerController(), GetOwner());;
		}
	}

	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UGun::FireFlare(FHitResult& Hit, FVector& ShotDirection)
{
	if(GunTrace(Hit, ShotDirection))
	{
		
		DrawDebugSphere(GetWorld(),Hit.Location,10,10,FColor::Yellow,true,5);
		AActor* HitActor = Hit.GetActor();
		if(HitActor != nullptr)
		{
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, GetOwnerController(), GetOwner());;
		}
	}

	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void UGun::FirePiercing(FHitResult& Hit, FVector& ShotDirection)
{
	if(GunTrace(Hit, ShotDirection))
	{
		
		DrawDebugSphere(GetWorld(),Hit.Location,10,10,FColor::Purple,true,5);
		AActor* HitActor = Hit.GetActor();
		if(HitActor != nullptr)
		{
			FPointDamageEvent DamageEvent(Damage, Hit, ShotDirection, nullptr);
			HitActor->TakeDamage(Damage, DamageEvent, GetOwnerController(), GetOwner());;
		}
	}

	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
	}
	
	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}




