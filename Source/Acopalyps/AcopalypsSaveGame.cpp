// Fill out your copyright notice in the Description page of Project Settings.


#include "AcopalypsSaveGame.h"

#include "AcopalypsCharacter.h"
#include "CombatManager.h"
#include "EnemyAICharacter.h"
#include "AI/NavigationSystemBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

UAcopalypsSaveGame::UAcopalypsSaveGame() :
	SlotName("test_save"),
	UserIndex(0) {}

void UAcopalypsSaveGame::SaveGameInstance(TArray<AActor*> Actors)
{
	WorldName = FName(FNavigationSystem::GetWorldFromContextObject(Actors.Last())->GetName());
	for( AActor* Actor : Actors )
	{
		if( Actor != nullptr && Actor->ActorHasTag("Player") )
		{
			const AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(Actor);
			PlayerInstance = {
				Actor,
				Player->GetTransform(),
				Player->GetVelocity(),
				Player->GetFirstPersonCameraComponent()->GetComponentRotation(),
				Player->HealthComponent->IsDead(),
				Player->HealthComponent->GetHealth(),
				Player->Gun->CurrentMag
			};
			UE_LOG(LogTemp, Display, TEXT("Player: %p"), PlayerInstance.Object)
			UE_LOG(LogTemp, Display, TEXT("Player Transform: %s"), *PlayerInstance.Transform.ToString())
		}
		else if( Actor != nullptr && (Actor->ActorHasTag("Enemy") && !Cast<AEnemyAICharacter>(Actor)->IsDead()) )
		{
			if( const AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(Actor) )
			{
				ActorsInWorld.Add({
					Actor,
					Enemy->GetTransform(),
					Enemy->GetVelocity(),
					Enemy->GetController()->GetControlRotation(),
					Enemy->IsDead(),
					Enemy->HealthComponent->GetHealth(),
					Enemy->Gun->CurrentMag,
					Enemy->Manager
				});
				UE_LOG(LogTemp, Display, TEXT("Enemy: %p"), ActorsInWorld.Last().Object)
				UE_LOG(LogTemp, Display, TEXT("Enemy Transform: %s"), *ActorsInWorld.Last().Transform.ToString())
			}
		}
		else if( Actor->IsRootComponentMovable() )
		{
			ActorsInWorld.Add({
				Actor,
				Actor->GetActorTransform(),
				Actor->GetVelocity()
			});
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Green, TEXT("Game Saved!"));
}

void UAcopalypsSaveGame::LoadGameInstance()
{
	if( WorldName != FName(FNavigationSystem::GetWorldFromContextObject(ActorsInWorld.Last().Object)->GetName()) )
	{
		UGameplayStatics::OpenLevel(this, WorldName, false);
	}
	for( const FActorInstance Actor : ActorsInWorld )
	{
		if( Actor.Object != nullptr )
		{
			if( Actor.Object->ActorHasTag("Player") )
			{
				AAcopalypsCharacter* Player = Cast<AAcopalypsCharacter>(Actor.Object);
				Player->SetActorTransform(Actor.Transform);
				Player->GetCharacterMovement()->Velocity = Actor.Velocity;
				if( Actor.ControllerRotation.IsSet() )
				{
					Player->GetController()->SetControlRotation(Actor.ControllerRotation.GetValue());
				}
				if( Actor.Health.IsSet() )
				{
					Player->HealthComponent->SetHealth(Actor.Health.GetValue());
				}
				if( Actor.GunMag.IsSet() )
				{
					Player->Gun->CurrentMag = Actor.GunMag.GetValue();
				}
			}
			else if( Actor.Object->ActorHasTag("Enemy") )
			{
				AEnemyAICharacter* Enemy = Cast<AEnemyAICharacter>(Actor.Object);
				if( Enemy->IsDead() )
				{
					if( !Actor.bIsDead )
					{
						//AEnemyAICharacter* RespawnedEnemy = GetWorld()->SpawnActor<AEnemyAICharacter>(
						//	AEnemyAICharacter::StaticClass(),
						//	Actor.Transform.GetLocation(),
						//	Actor.Transform.Rotator()
						//	);
						if( Actor.Health.IsSet() )
						{
							Enemy->HealthComponent->SetHealth(Actor.Health.GetValue());
						}
						if( Actor.GunMag.IsSet() )
						{
							Enemy->Gun->CurrentMag = Actor.GunMag.GetValue();
						}
						if( Actor.Manager.IsSet() )
						{
							Enemy->Manager = Actor.Manager.GetValue();
							Enemy->Manager->AddEnemy(Enemy);
						}
					}
				} else
				{
					Enemy->SetActorTransform(Actor.Transform);
					Enemy->GetCharacterMovement()->Velocity = Actor.Velocity;
					if( Actor.Health.IsSet() )
					{
						Enemy->HealthComponent->SetHealth(Actor.Health.GetValue());
					}
					if( Actor.GunMag.IsSet() )
					{
						Enemy->Gun->CurrentMag = Actor.GunMag.GetValue();
					}
					if( Actor.Manager.IsSet() )
					{
						Enemy->Manager = Actor.Manager.GetValue();
					}
				}
			} else if( Actor.Object->Owner == nullptr )
			{
				Actor.Object->SetActorTransform(Actor.Transform);
				if( UPrimitiveComponent* Comp = Cast<UPrimitiveComponent>(Actor.Object) )
				{
					Comp->SetSimulatePhysics(false);
					Comp->SetSimulatePhysics(true);
				}
			}
		}
	}
	GEngine->AddOnScreenDebugMessage(-1, 6.f, FColor::Green, TEXT("Game Loaded."));
}
