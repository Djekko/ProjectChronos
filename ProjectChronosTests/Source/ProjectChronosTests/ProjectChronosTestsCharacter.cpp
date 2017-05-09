// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ProjectChronosTests.h"
#include "ProjectChronosTestsCharacter.h"
#include "ProjectChronosTestsProjectile.h"
#include "CPP_Singularity.h"
#include "CPP_TimeDihalationAreaGhost.h"
#include "CPP_TimeDihalationArea.h"
#include "CPP_Shockwave.h"
#include "CPP_Door.h"
#include "Animation/AnimInstance.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

#define OUT
#define CustomOverlapTrace ECC_GameTraceChannel2

//////////////////////////////////////////////////////////////////////////
// AProjectChronosTestsCharacter

AProjectChronosTestsCharacter::AProjectChronosTestsCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 60.f;
	BaseLookUpRate = 60.f;

	//Ladder Bool
	bCanClimbLadder = false;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	DamageShake = UCameraShake::StaticClass()->GetDefaultObject<UCameraShake>();
	DamageShake->OscillationDuration = 0.5f; //negative value will run forever
	DamageShake->RotOscillation.Pitch.Amplitude = 1.0f;
	DamageShake->RotOscillation.Pitch.Frequency = 25.f;
	DamageShake->RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

	DamageShake->RotOscillation.Yaw.Amplitude = 1.0f;
	DamageShake->RotOscillation.Yaw.Frequency = 25.f;
	DamageShake->RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;

	FindBlueprints();
/*
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	
	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;

	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);
*/
	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

void AProjectChronosTestsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	//FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint")); //Attach gun mesh component to Skeleton, doing it here because the skelton is not yet created in the constructor
}

void AProjectChronosTestsCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	///timer
	Timer += DeltaTime;

	if (Timer >= 1.f)
	{
		Timer -= 1.f;

		if (Seconds == 0)
		{
			if (Minutes == 0)
			{
				UGameplayStatics::OpenLevel(GetWorld(), FName("MainMenu"), true, "");
			}
			else
			{
				Minutes--;
				Seconds = 59;
			}
		}
		else
		{
			Seconds--;
		}
	}


	///sprint
	bCanSprint = true;
	if (GetCapsuleComponent()->GetPhysicsLinearVelocity() == FVector(0.f, 0.f, 0.f))
	{
		bCanSprint = false;
		if (bIsSprinting)
		{
			OnSprintStop();
		}
	}

	///crouch
	if (!bCanUnCrouch)
	{
		TickTryUnCrouch();
	}

	///Energy
	if (bEnergyDecaying)
	{
		DecayEnergy(DeltaTime);
	}
	if (bEnergyRegenerating)
	{
		RegenerateEnergy(DeltaTime);
	}

	///Rewind
	if (PlayerEnergy <= 0.f)
	{
		//TODO EnableInput(GetWorld()->GetFirstPlayerController());
		StopTimeRewinding();
	}
	else
	{
		//TODO DisableInput(GetWorld()->GetFirstPlayerController());
		ProcessPlayerState();
	}

	///Abilities
	LineTrace();
	if (bReverseButtonPressed)
	{
		replayAbilityNum = 0;

		TArray<float> newArray;
		newArray.Add(0.f);
		SpawnArrayTemp = newArray;

		bAbilityReplayFlag = true;

		RespawnAbilitiesWhileRewinding();
	}
	else
	{
		respawnAbilityNum = 0;

		ReplayAbilitiesAfterRewinding();
	}

	///damage
	if (bIsInDamageArea)
	{
		AddPeriodicDamage();
	}
	else
	{
		if (bFirstTimeDamage == false)
		{
			bEnergyRegenerating = true;
			bFirstTimeDamage = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AProjectChronosTestsCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	// set up gameplay key bindingsww
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	//InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AProjectChronosTestsCharacter::TouchStarted);
	if (EnableTouchscreenMovement(InputComponent) == false)
	{
		InputComponent->BindAction("Fire", IE_Pressed, this, &AProjectChronosTestsCharacter::OnFire);
	}

	//AbilityBinds
	InputComponent->BindAction("ToggleAbilityWheelUp", IE_Pressed, this, &AProjectChronosTestsCharacter::ChangeAbilityDown);
	InputComponent->BindAction("ToggleAbilityWheelDown", IE_Pressed, this, &AProjectChronosTestsCharacter::ChangeAbilityUp);
	InputComponent->BindAction("ActivateAbility1", IE_Pressed, this, &AProjectChronosTestsCharacter::ActivateAbility1);
	InputComponent->BindAction("ActivateAbility2", IE_Pressed, this, &AProjectChronosTestsCharacter::ActivateAbility2);
	InputComponent->BindAction("ActivateAbility3", IE_Pressed, this, &AProjectChronosTestsCharacter::ActivateAbility3);
	InputComponent->BindAction("ActivateAbility4", IE_Pressed, this, &AProjectChronosTestsCharacter::ActivateAbility4);

	//Sprint Binds
	InputComponent->BindAction("Sprint", IE_Pressed, this, &AProjectChronosTestsCharacter::OnSprint);
	InputComponent->BindAction("Sprint", IE_Released, this, &AProjectChronosTestsCharacter::OnSprintStop);

	//Crouch Binds
	InputComponent->BindAction("Crouch", IE_Pressed, this, &AProjectChronosTestsCharacter::PlayerCrouch);
	InputComponent->BindAction("Crouch", IE_Released, this, &AProjectChronosTestsCharacter::PlayerUnCrouch);

	//Test Button
	InputComponent->BindAction("Toggle Controls", IE_Pressed, this, &AProjectChronosTestsCharacter::TestCode);
	InputComponent->BindAxis("MoveForward", this, &AProjectChronosTestsCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AProjectChronosTestsCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &AProjectChronosTestsCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &AProjectChronosTestsCharacter::LookUpAtRate);
}

void AProjectChronosTestsCharacter::TestCode()
{
	int count = 0;

	int size = 5;
	
	float m_DataX[25];
	float m_DataY[25];

	for (int x = 0; x < 25; x++)
	{
		m_DataX[x] = (float)x;
		m_DataY[x] = -1.f;
	}

	int halfSize = ((size*size)+1) / 2;

	for (int i = 0; i < size; i++)
	{
		int iTimesSize = i*size;
		for (int j = 0; j < size; j++)
		{
			int ArrayNumY = iTimesSize + j;
			int ArrayNumX = j*size + i;

			m_DataY[ArrayNumY] = m_DataX[ArrayNumX];
			count++;

			UE_LOG(LogTemp, Warning, TEXT("Ynum = %d, Xnum = %d, m_DataY = %f, count = %d, halfSize = %d"), 
				ArrayNumY, 
				ArrayNumX, 
				m_DataY[iTimesSize + j], 
				count,
				halfSize
			);
		}
	}
}

void AProjectChronosTestsCharacter::LineTrace()
{
	/// get the player view point this tick
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotation;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotation
	);
	
	FVector LineTraceEnd = PlayerViewPointLocation + PlayerViewPointRotation.Vector() * reach;
	/// Draw a red trace in the world to visualise
	//DrawDebugLine(
	//	GetWorld(),	// get a world
	//	PlayerViewPointLocation,	// line trace start
	//	LineTraceEnd,	// line trace end
	//	FColor(255, 0, 0),	// line colour
	//	false,	// persistence
	//	0.f,	// life time
	//	0.f,	// Depth
	//	5.f		// thickness
	//);

	/// setup querry params
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	/// LineTrace/Ray-Cast out to reach distance
	FHitResult LineTraceHit;
	GetWorld()->LineTraceSingleByChannel(
		OUT LineTraceHit,
		PlayerViewPointLocation,
		LineTraceEnd,
		ECC_Visibility,
		TraceParams
	);

	bLineTraceHitSomething = false;

	/// log actor hit
	LineTraceActorHit = LineTraceHit.GetActor();
	LineTraceComponentHit = LineTraceHit.GetComponent();
	if (LineTraceComponentHit)
	{
		bLineTraceHitSomething = true;
		if (bLineTraceHitSomething)
		{
			//is actor hit valid?
			if (LineTraceActorHit->IsValidLowLevel())
			{
				//does the actor hit have a valid tag
				bool bHasAgeTag = LineTraceActorHit->Tags.Contains(FName("AgeableObject"));
				bool bHasDoorTag = LineTraceActorHit->Tags.Contains(FName("Door"));
				bool bHasReageTag = LineTraceActorHit->Tags.Contains(FName("ReageableButton"));
				if (bHasAgeTag || bHasDoorTag || bHasReageTag)
				{
					if (!bInteractionQueHudVisable)
					{
						//TODO if no add que hud to viewport
						DisplayQueHud();

						bInteractionQueHudVisable = true;
					}
				}

				bool bHasLadderTag = LineTraceComponentHit->ComponentTags.Contains(FName("Ladder"));

				bIsFacingLadder = false;
				if (bHasLadderTag)
				{
					bIsFacingLadder = true;
				}
			}	
		}
		//UE_LOG(LogTemp, Warning, TEXT("Line Trace Hit: %s"), *(LineTraceComponentHit->GetName()));
	}
	else
	{
		//if que hud is visable
		if (bInteractionQueHudVisable)
		{
			//TODO remove que hud from parent
			RemoveQueHud();

			//set que hud visable to false
			bInteractionQueHudVisable = false;
		}
	}
}

void AProjectChronosTestsCharacter::ChangeAbilityUp()
{
	if (AbilityNumber == 3)
	{
		AbilityNumber = 0;
	}
	else
	{
		AbilityNumber++;
	}
}

void AProjectChronosTestsCharacter::ChangeAbilityDown()
{
	if (AbilityNumber == 0)
	{
		AbilityNumber = 3;
	}
	else
	{
		AbilityNumber--;
	}
}

void AProjectChronosTestsCharacter::ActivateAbility1(){ AbilityNumber = 0; }

void AProjectChronosTestsCharacter::ActivateAbility2(){ AbilityNumber = 1; }

void AProjectChronosTestsCharacter::ActivateAbility3(){ AbilityNumber = 2; }

void AProjectChronosTestsCharacter::ActivateAbility4(){ AbilityNumber = 3; }

bool AProjectChronosTestsCharacter::ReduceEnergy(float reductionAmount)
{
	bool bHasReduced = false;
	if (PlayerEnergy >= reductionAmount)
	{
		PlayerEnergy -= reductionAmount;
		bHasReduced = true;
	}
	return bHasReduced;
}

void AProjectChronosTestsCharacter::DecayEnergy(float DeltaSeconds)
{
	if (PlayerEnergy <= 0.f)
	{
		PlayerEnergy = 0.f;
		bEnergyDecaying = false;
	}
	else
	{
		PlayerEnergy -= (DeltaSeconds*100)/3;
	}
}

void AProjectChronosTestsCharacter::RegenerateEnergy(float DeltaSeconds)
{
	if (PlayerEnergy >= PlayerMaxEnergy)
	{
		PlayerEnergy = PlayerMaxEnergy;
		bEnergyRegenerating = false;
	}
	else
	{
		PlayerEnergy += (DeltaSeconds * 100) / 3;
	}
}

void AProjectChronosTestsCharacter::AddPeriodicDamage()
{
	bEnergyDecaying = false;
	bEnergyRegenerating = false;

	float now = UGameplayStatics::GetRealTimeSeconds(GetWorld());

	if (now - damageStartTime >= 1.f || bFirstTimeDamage)
	{
		if (bFirstTimeDamage)
		{
			bFirstTimeDamage = false;
		}
		else
		{
			damageStartTime += 1.f;
		}

		//UE_LOG(LogTemp, Warning, TEXT("Apply Damage!"), );

		ShakeCam(DamageLocation);


		if (PlayerEnergy == 0.f)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Kill Player"), );
			GameOver();
		}
		else if (PlayerEnergy <= 10.f)
		{
			PlayerEnergy = 0.f;
		}
		else
		{
			PlayerEnergy -= 10.f;
		}
	}
}

void AProjectChronosTestsCharacter::SpawnTimeVolumeGhost()
{
	UE_LOG(LogTemp, Warning, TEXT("Should Spawn Ghost"),);

	FVector CameraLocation = FirstPersonCameraComponent->GetComponentLocation();
	FVector CameraForwardVector = FirstPersonCameraComponent->GetComponentQuat().GetForwardVector() * dihalationAreaDistance;

	FVector Location = CameraLocation + CameraForwardVector;
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TimeVolumeGhostRef = GetWorld()->SpawnActor<ACPP_TimeDihalationAreaGhost>
		(ACPP_TimeDihalationAreaGhost::StaticClass()
			, Location
			, Rotation
			, SpawnInfo
			);
}

void AProjectChronosTestsCharacter::SpawnTimeVolume()
{
	if (TimeVolumeRef->IsValidLowLevel())
	{
		TimeVolumeRef->Destroy();
	}

	FVector CameraLocation = FirstPersonCameraComponent->GetComponentLocation();
	FVector CameraForwardVector = FirstPersonCameraComponent->GetComponentQuat().GetForwardVector() * dihalationAreaDistance;

	FVector Location = CameraLocation + CameraForwardVector;
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TimeVolumeRef = GetWorld()->SpawnActor<ACPP_TimeDihalationArea>
		(ACPP_TimeDihalationArea::StaticClass()
			, Location
			, Rotation
			, SpawnInfo
			);

	ActorsSpawned.Add(TimeVolumeRef);

	float time = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	SpawnedTime.Add(time);

	FTransform newTransform;
	newTransform.SetLocation(Location);
	SpawnedTransform.Add(newTransform);
}

void AProjectChronosTestsCharacter::RemoveTimeVolumeGhost()
{
	bTimeVolumeGhostSpawned = false;
	if (TimeVolumeGhostRef->IsValidLowLevel())
	{
		TimeVolumeGhostRef->Destroy();
	}
}

void AProjectChronosTestsCharacter::SpawnSingularity()
{
	FVector CameraLocation = FirstPersonCameraComponent->GetComponentLocation();
	FVector CameraForwardVector = FirstPersonCameraComponent->GetComponentQuat().GetForwardVector() * SingularityDistFromPlayer;

	FVector Location = CameraLocation + CameraForwardVector;
	FRotator Rotation(0.0f, 0.0f, 0.0f);
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	ACPP_Singularity* newSingularity = GetWorld()->SpawnActor<ACPP_Singularity>
		(ACPP_Singularity::StaticClass()
			, Location
			, Rotation
			, SpawnInfo);

	ActorsSpawned.Add(newSingularity);

	float time = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	SpawnedTime.Add(time);

	FTransform newTransform;
	newTransform.SetLocation(Location);
	SpawnedTransform.Add(newTransform);
}

void AProjectChronosTestsCharacter::SpawnShockwave()
{
	//create spawning perameters
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	//get the spawn transformation
	FTransform newTransform = FirstPersonCameraComponent->GetComponentTransform();

	//spawn the shockwave
	ShockwaveRef = GetWorld()->SpawnActor<ACPP_Shockwave>
		(ACPP_Shockwave::StaticClass()
			, newTransform
			, SpawnInfo);

	//add actor arrayfor rewinding time
	ActorsSpawned.Add(ShockwaveRef);

	//add the spawn time to an array for rewinding time 
	float time = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	SpawnedTime.Add(time);
	
	//add transform for the array for rewinding time 
	SpawnedTransform.Add(newTransform);
}

void AProjectChronosTestsCharacter::ShockwaveEffect()
{
	FVector traceStart = FirstPersonCameraComponent->GetComponentLocation();
	FVector traceEnd = FirstPersonCameraComponent->GetComponentQuat().GetForwardVector() * 400.f;

	traceEnd += traceStart;

	/*DrawDebugSphere(
		GetWorld(),
		traceStart,
		100.f,
		32,
		FColor(255, 0, 0) ,
		false ,
		5.f
	);

	DrawDebugSphere(
		GetWorld(),
		traceEnd,
		100.f,
		32,
		FColor(0, 255, 0),
		false,
		5.f
	);*/

	FCollisionShape TraceSphere = FCollisionShape::MakeSphere(100.f); //1M Radius
	TArray<FHitResult> OutResults;
	GetWorld()->SweepMultiByChannel(
		OUT OutResults ,
		traceStart ,
		traceEnd ,
		FQuat::Identity ,
		CustomOverlapTrace,
		TraceSphere
	);

	
	for (int x = 0; x < OutResults.Num(); x++)
	{
		AActor* actorHit = OutResults[x].GetActor();
		UActorComponent* CompHit = OutResults[x].GetComponent();
		//UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(CompHit);
		
		
		if (CompHit->ComponentTags.Contains(FName("AIUnit"))) 
		{
			UE_LOG(LogTemp, Warning, TEXT("backflips %s"), *actorHit->GetName());
			RagdollAI(actorHit);

			return;
		}
		if (CompHit->GetClass() == UStaticMeshComponent::StaticClass() || CompHit->GetClass() == USkeletalMeshComponent::StaticClass())
		{
			if (CompHit->GetClass() == UStaticMeshComponent::StaticClass())
			{
				UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(CompHit);

				if (actorHit && actorHit != this && actorHit != ShockwaveRef && MeshComp->IsSimulatingPhysics())
				{
					//UE_LOG(LogTemp, Warning, TEXT("Multi Trace Hit: %s"), *actorHit());

					if (actorHit->IsValidLowLevel() && actorHit != this)
					{
						if (actorHit->Tags.Contains(FName("CanBreak")))
						{
							actorHit->Tags.Add(FName("BreakTimeReplay"));
						}

						FVector MeshLocation = MeshComp->GetComponentLocation();
						FVector CamLocation = FirstPersonCameraComponent->GetComponentLocation();

						FVector NewVelocity;

						// applys the correct force dependant on if the object is in the time dilation area or not

						if (MeshComp->ComponentTags.Contains(FName("IsTimeDilated")))
						{
							NewVelocity = (MeshLocation - CamLocation) * (shockwaveForce / 20);
						}
						else
						{
							NewVelocity = (MeshLocation - CamLocation) * shockwaveForce;
						}

						MeshComp->SetPhysicsLinearVelocity(NewVelocity, true, "");

						float a = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 360))) - 180;
						float b = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 360))) - 180;
						float c = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 360))) - 180;

						FVector newAngularVelocity = (FVector(a, b, c));

						MeshComp->SetPhysicsAngularVelocity(newAngularVelocity, false, "");

					}
				}
			}
			if (CompHit->GetClass() == USkeletalMeshComponent::StaticClass())
			{
				USkeletalMeshComponent* MeshComp = Cast<USkeletalMeshComponent>(CompHit);

				if (actorHit && actorHit != this && actorHit != ShockwaveRef && MeshComp->IsSimulatingPhysics())
				{
					//UE_LOG(LogTemp, Warning, TEXT("Multi Trace Hit: %s"), *actorHit());

					if (actorHit->IsValidLowLevel() && actorHit != this)
					{
						if (actorHit->Tags.Contains(FName("CanBreak")))
						{
							actorHit->Tags.Add(FName("BreakTimeReplay"));
						}

						FVector MeshLocation = MeshComp->GetComponentLocation();
						FVector CamLocation = FirstPersonCameraComponent->GetComponentLocation();

						FVector NewVelocity;

						// applys the correct force dependant on if the object is in the time dilation area or not

						if (MeshComp->ComponentTags.Contains(FName("IsTimeDilated")))
						{
							NewVelocity = (MeshLocation - CamLocation) * ((shockwaveForce / 2) / 20);
						}
						else
						{
							NewVelocity = (MeshLocation - CamLocation) * (shockwaveForce/2);
						}

						MeshComp->SetPhysicsLinearVelocity(NewVelocity, true, "");

						float a = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 360))) - 180;
						float b = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 360))) - 180;
						float c = (static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 360))) - 180;

						FVector newAngularVelocity = (FVector(a, b, c));

						MeshComp->SetPhysicsAngularVelocity(newAngularVelocity, false, "");

					}
				}
			}
		}
	}
}

void AProjectChronosTestsCharacter::FindBlueprints()
{
	/*static ConstructorHelpers::FObjectFinder<UBlueprint>FoundSingularityBP(TEXT("/Game/ChronosTests/BlackHole/BP_CPP_Singularity"));
	if (FoundSingularityBP.Object)
	{
		SingularityBP = (UClass*)FoundSingularityBP.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint>FoundTimeDihalationAreaBP(TEXT("/Game/ChronosTests/FreezingObjects/BP_CPP_TimeDihalationArea"));
	if (FoundTimeDihalationAreaBP.Object)
	{
		TimeDihalationAreaBP = (UClass*)FoundTimeDihalationAreaBP.Object->GeneratedClass;
	}

	static ConstructorHelpers::FObjectFinder<UBlueprint>FoundShockwaveBP(TEXT("/Game/ChronosTests/CoriolisPulse/BP_CPP_Shockwave"));
	if (FoundShockwaveBP.Object)
	{
		ShockwaveBP = (UClass*)FoundShockwaveBP.Object->GeneratedClass;
	}*/

	static ConstructorHelpers::FObjectFinder<UBlueprint>FoundSwatRagdollBP(TEXT("/Game/ChronosTests/HeavySoldier/BP_SwatRagdoll"));
	if (FoundSwatRagdollBP.Object)
	{
		SwatRagdollBP = (UClass*)FoundSwatRagdollBP.Object->GeneratedClass;
	}
}

void AProjectChronosTestsCharacter::ProcessPlayerState()
{
	ProcessPlayerTransform();
	ProcessPlayerVelocity();
	ProcessPlayerRotation();
}

void AProjectChronosTestsCharacter::ProcessPlayerTransform()
{
	if (bReverseButtonPressed)
	{
		if (PlayerTransArray.Num() >= 1)
		{
			int32 lastIndex = PlayerTransArray.Num() - 1;
			FTransform newTransform = PlayerTransArray[lastIndex];

			GetCapsuleComponent()->SetWorldTransform(newTransform);

			if (lastIndex > 0)
			{
				ReplayPlayerTransArray.Add(newTransform);

				PlayerTransArray.RemoveAt(lastIndex, true);
			}
		}
	}
	else
	{
		if (PlayerTransArray.Num() >= 350)
		{
			PlayerTransArray.RemoveAt(0, true);
		}

		PlayerTransArray.Add(GetCapsuleComponent()->GetComponentTransform());
	}
}

void AProjectChronosTestsCharacter::ProcessPlayerVelocity()
{
	bool bResetVelocity = false;

	if (bReverseButtonPressed)
	{
		bResetVelocity = true;

		if (PlayerVelocArray.Num() >= 1)
		{
			int32 lastIndex = PlayerVelocArray.Num() - 1;
			FTransform newTransform = PlayerVelocArray[lastIndex];

			FVector newLinearVeloc = newTransform.GetLocation() * -1;
			FVector newAngularVeloc = newTransform.GetScale3D() * -1;

			GetCapsuleComponent()->SetPhysicsLinearVelocity(newLinearVeloc);
			GetCapsuleComponent()->SetPhysicsAngularVelocity(newAngularVeloc);

			if (lastIndex > 0)
			{
				ReplayPlayerVelocArray.Add(newTransform);
				PlayerVelocArray.RemoveAt(lastIndex, true);
			}
		}
	}
	else
	{
		if (bResetVelocity)
		{
			GetCapsuleComponent()->SetPhysicsLinearVelocity(GetCapsuleComponent()->GetPhysicsLinearVelocity() * -1);
			GetCapsuleComponent()->SetPhysicsAngularVelocity(GetCapsuleComponent()->GetPhysicsAngularVelocity() * -1);

			bResetVelocity = false;
		}

		if (PlayerVelocArray.Num() >= 350)
		{
			PlayerVelocArray.RemoveAt(0, true);
		}

		FTransform newVelocities;
		newVelocities.SetLocation(GetCapsuleComponent()->GetPhysicsLinearVelocity());
		newVelocities.SetScale3D(GetCapsuleComponent()->GetPhysicsAngularVelocity());

		PlayerVelocArray.Add(newVelocities);
	}
}

void AProjectChronosTestsCharacter::ProcessPlayerRotation()
{
	if (bReverseButtonPressed)
	{
		if (PlayerRotArray.Num() >= 1)
		{
			int32 lastIndex = PlayerRotArray.Num() - 1;
			FRotator newRotation = PlayerRotArray[lastIndex];

			GetController()->SetControlRotation(newRotation);
			if (lastIndex > 0)
			{
				ReplayPlayerRotArray.Add(newRotation);
				PlayerRotArray.RemoveAt(lastIndex, true);
			}
		}
	}
	else
	{
		if (PlayerRotArray.Num() >= 350)
		{
			PlayerRotArray.RemoveAt(0, true);
		}

		PlayerRotArray.Add(GetController()->GetControlRotation());
	}
}

void AProjectChronosTestsCharacter::RespawnAbilitiesWhileRewinding()
{
	int32 lastIndex = DestroyedTime.Num() - 1;

	bool bAbilitiesToRespawn = (lastIndex - respawnAbilityNum) < 0;
	bool bValidArray = DestroyedTime.Num() > 0;

	if (!bAbilitiesToRespawn && bValidArray)
	{
		float realTimeOffset = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - rewindStartTime;
		float timeToNextAbility = rewindStartTime - DestroyedTime[lastIndex - respawnAbilityNum];
		bool bTimeToRespawn = realTimeOffset > timeToNextAbility;

		if (bTimeToRespawn)
		{
			respawnAbilityNum++;

			int32 lastIndexActor = ActorsDestroyed.Num() - 1;
			int32 arrayElemntIndex = lastIndexActor - (respawnAbilityNum - 1);

			FTransform spawnTransform = DestroyedTransform[arrayElemntIndex];

			UE_LOG(LogTemp, Warning, TEXT("Class = %s"), *ActorsDestroyed[arrayElemntIndex]->GetName());

			GetWorld()->SpawnActor<AActor>(
				ActorsDestroyed[arrayElemntIndex],
				spawnTransform
				);
		}
	}
}

void AProjectChronosTestsCharacter::ReplayAbilitiesAfterRewinding()
{
	if (rewindEndTime != rewindStartTime && bAbilityReplayFlag)
	{
		bAbilityReplayFlag = false;

		SpawnArrayTemp.RemoveAt(0, true);

		for (int x = 0; x < SpawnedTime.Num(); x++)
		{
			bool bSpawnedWhileRewinding = rewindStartTime - (rewindEndTime - rewindStartTime) <= SpawnedTime[x];
			bool bIsValidTime = SpawnedTime[x] >= 0.f;
			if (bSpawnedWhileRewinding && bIsValidTime)
			{
				SpawnArrayTemp.Add(SpawnedTime[x]);
			}
		}
	}

	float timeSinceRewindEnd = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - rewindEndTime;
	float timeSpentRewinding = rewindEndTime - rewindStartTime;

	bool bReplayTimePassed = timeSinceRewindEnd < timeSpentRewinding;
	bool bReplayNumIsInArrayBounds = replayAbilityNum < SpawnArrayTemp.Num();
	bool bIsValidArray = SpawnArrayTemp.Num() > 0;

	if (bReplayTimePassed && bReplayNumIsInArrayBounds && bIsValidArray)
	{
		float A = UGameplayStatics::GetRealTimeSeconds(GetWorld()) - rewindEndTime;
		float B = rewindStartTime - (rewindEndTime - rewindStartTime);
		float C = SpawnArrayTemp[replayAbilityNum] - B;

		bool bIsRightTimeToSpawn = A > C;

		if (bIsRightTimeToSpawn)
		{
			replayAbilityNum++;

			int32 lastIndex = SpawnedTransform.Num() - 1;
			int32 indexElement = (lastIndex - SpawnArrayTemp.Num()) + replayAbilityNum;

			GetWorld()->SpawnActor<AActor>(
				ActorsDestroyed[indexElement],
				SpawnedTransform[indexElement]
				);
		}
	}
}

void AProjectChronosTestsCharacter::DoorDispatch()
{
	for (TObjectIterator<ACPP_Door> Itr; Itr; ++Itr)
	{
		if (Itr->IsA(ACPP_Door::StaticClass()))
		{
			ACPP_Door* actorClass = *Itr;
			actorClass->OpenCloseDoor();
		}
	}
}

void AProjectChronosTestsCharacter::OnFire()
{
	if (TimeVolumeGhostRef->IsValidLowLevel())
	{
		TimeVolumeGhostRef->Destroy();
	}

	if (!bReverseButtonPressed)
	{
		switch (AbilityNumber)
		{
		case 0:		//shockwave
			SpawnShockwave();
			ShockwaveEffect();
			break;
		case 1:		//time volume
			if (PlayerEnergy > 60.f)
			{
				if (bTimeVolumeGhostSpawned)
				{
					if (ReduceEnergy(60.f))
					{
						bEnergyDecaying = false;
						bEnergyRegenerating = true;
						bTimeVolumeGhostSpawned = false;
						SpawnTimeVolume();
					}
				}
				else
				{
					bTimeVolumeGhostSpawned = true;
					SpawnTimeVolumeGhost();
				}
			}
			
			break;
		case 2:		//singularity
			if (ReduceEnergy(60.f))
			{
				bEnergyDecaying = false;
				bEnergyRegenerating = true;
				SpawnSingularity();
			}
			break;
		case 3:		//rewind time
			AddRewindFilter();
			break;
		}
	}
}

void AProjectChronosTestsCharacter::OnSprint()
{
	if (bCanSprint)
	{
		bIsSprinting = true;
		if (!bIsCrouching && !bCanClimbLadder)
		{
			GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;
			bEnergyDecaying = true;
			bEnergyRegenerating = false;
		}
	}
}

void AProjectChronosTestsCharacter::OnSprintStop()
{
	bIsSprinting = false;
	if (!bIsCrouching && !bCanClimbLadder)
	{
		GetCharacterMovement()->MaxWalkSpeed = walkSpeed;
		bEnergyDecaying = false;
		bEnergyRegenerating = true;
	}
}

void AProjectChronosTestsCharacter::PlayerCrouch()
{
	bCanUnCrouch = true;
	bIsCrouching = true;

	GetCharacterMovement()->MaxWalkSpeed = crouchSpeed;

	PlayCrouchAnimation();
}

void AProjectChronosTestsCharacter::PlayerUnCrouch()
{
	FVector Location = GetCapsuleComponent()->GetComponentLocation();
	FVector traceStart = Location + FVector(0.f, 0.f, 96.01f);
	FVector traceEnd = Location + FVector(0.f, 0.f, 96.f);

	FQuat Orientation = GetCapsuleComponent()->GetComponentQuat();

	//FCollisionShape TraceBox = FCollisionShape::MakeBox(FVector(37.f, 37.f, 48.f));
	FCollisionShape TraceSphere = FCollisionShape::MakeCapsule(50.f, 96.f);

	/*DrawDebugCapsule(
		GetWorld(),
		traceStart,
		96.f,
		50.f,
		Orientation,
		FColor(255, 0, 0),
		false,
		5.f
	);*/

	FHitResult OutResults;
	GetWorld()->SweepSingleByChannel(
		OUT OutResults,
		traceStart,
		traceEnd,
		Orientation,
		ECC_Visibility,
		TraceSphere
	);

	if (OutResults.GetComponent())
	{
		bCanUnCrouch = false;
	}
	else
	{
		PlayerStand();
	}
}

void AProjectChronosTestsCharacter::PlayerStand()
{
	bIsCrouching = false;

	GetCharacterMovement()->MaxWalkSpeed = walkSpeed;

	PlayStandAnimation();
}

void AProjectChronosTestsCharacter::TickTryUnCrouch()
{
	FVector Location = GetCapsuleComponent()->GetComponentLocation();
	FVector traceStart = Location + FVector(0.f, 0.f, 48.01f);
	FVector traceEnd = Location + FVector(0.f, 0.f, 48.f);

	FQuat Orientation = GetCapsuleComponent()->GetComponentQuat();

	FCollisionShape TraceCapsule = FCollisionShape::MakeCapsule(42.f, 96.f);

	/*DrawDebugCapsule(
		GetWorld(),
		traceEnd,
		96.f,
		42.f,
		Orientation,
		FColor(0, 255, 255),
		false,
		0.1f
	);*/

	FHitResult OutResults;
	GetWorld()->SweepSingleByChannel(
		OUT OutResults,
		traceStart,
		traceEnd,
		Orientation,
		ECC_Visibility,
		TraceCapsule
	);

	if (!OutResults.GetComponent())
	{
		bCanUnCrouch = true;
		PlayerStand();
	}
}

void AProjectChronosTestsCharacter::UpdateCrouchAnimation(FVector newScale)
{
	GetCapsuleComponent()->SetWorldScale3D(newScale);
}

void AProjectChronosTestsCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void AProjectChronosTestsCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void AProjectChronosTestsCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y * BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void AProjectChronosTestsCharacter::MoveForward(float Value)
{
	if (!bCanClimbLadder)
	{
		if (Value != 0.0f)
		{
			// add movement in forward direction
			AddMovementInput(GetActorForwardVector(), Value);
		}
	}
	else {
		if (Value != 0.0f)
		{
			// add movement in upwards direction
			AddMovementInput(GetActorUpVector(), Value);
		}
	}
}

void AProjectChronosTestsCharacter::MoveRight(float Value)
{
	if (!bCanClimbLadder)
	{
		if (Value != 0.0f)
		{
			// add movement in that direction
			AddMovementInput(GetActorRightVector(), Value);
		}
	}
}

void AProjectChronosTestsCharacter::TurnAtRate(float Rate)
{
	//inversion of controls
	float multiplier = 1.f;
	if (bInvertedX)
	{
		multiplier = -1.f;
	}

	float timeMultiplier = 1.f;
	if (bPlayerInTimeVolume)
	{
		timeMultiplier = 0.05f;
	}
	// calculate delta for this frame from the rate information
	AddControllerYawInput(timeMultiplier * multiplier * Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AProjectChronosTestsCharacter::LookUpAtRate(float Rate)
{
	//inversion of controls
	float multiplier = 1.f;
	if (bInvertedY)
	{
		multiplier = -1.f;
	}

	float timeMultiplier = 1.f;
	if (bPlayerInTimeVolume)
	{
		timeMultiplier = 0.05f;
	}
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(timeMultiplier * multiplier * Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool AProjectChronosTestsCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if (FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &AProjectChronosTestsCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &AProjectChronosTestsCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &AProjectChronosTestsCharacter::TouchUpdate);
	}
	return bResult;
}
