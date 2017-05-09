//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#include "ProjectChronosTests.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_Shockwave.h"


// Sets default values
ACPP_Shockwave::ACPP_Shockwave()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetUpMesh();

	Shockwave->ComponentTags.Add(FName("SingularityExempt"));
}

// Called when the game starts or when spawned
void ACPP_Shockwave::BeginPlay()
{
	Super::BeginPlay();
	
	OwnerShock = this;

	CastToPlayer();

	SetShockwaveLocations();
}

// Called every frame
void ACPP_Shockwave::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if ((bSpawnedReversing && !PlayerRef->bReverseButtonPressed)
		|| (!bSpawnedReversing && PlayerRef->bReverseButtonPressed))
	{
		bReversingShockwave = true;

		ReverseShockwave(DeltaTime);
	}
	else
	{
		bSpawnedReversing = false;
	
		PlayShockwave(DeltaTime);
	}
}

void ACPP_Shockwave::SetUpMesh()
{
	struct FConstructorStatics {
		ConstructorHelpers::FObjectFinder<UStaticMesh>Torus;
		ConstructorHelpers::FObjectFinder<UMaterial>TorusMat;
		FConstructorStatics()
			: Torus(TEXT("/Game/StarterContent/Shapes/Shape_Torus"))
			, TorusMat(TEXT("/Game/ChronosTests/CoriolisPulse/M_DistortionFX"))
		{}
	};
	static FConstructorStatics ConstructorStatics;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	Shockwave = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TimeVolume"));
	Shockwave->SetStaticMesh(ConstructorStatics.Torus.Object);
	Shockwave->SetMaterial(0, ConstructorStatics.TorusMat.Object);
	Shockwave->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	Shockwave->SetCollisionProfileName(TEXT("OverlapAll"));
	Shockwave->bGenerateOverlapEvents = false;
	Shockwave->SetupAttachment(RootComponent);
}

void ACPP_Shockwave::CastToPlayer()
{
	//This is casting to the player
	PlayerRef = Cast<AProjectChronosTestsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	///this tests that the casting works
	//FString TestString = FString::SanitizeFloat(PlayerRef->testCasting);
	//UE_LOG(LogTemp, Warning, TEXT("The Cast Worked from reversable The Number is %s"), *TestString);
}

void ACPP_Shockwave::SetShockwaveLocations()
{
	FVector TempActorLocation = this->GetActorLocation();
	FVector TempForwardVector = this->GetActorQuat().GetForwardVector();

	FVector StartLocation = (TempForwardVector * 50.f) + TempActorLocation;
	ShockWaveStartLocation = StartLocation;

	FVector EndLocation = (TempForwardVector * 800.f) + TempActorLocation;
	ShockWaveEndLocation = EndLocation;
}

void ACPP_Shockwave::PlayShockwave(float deltaTime)
{
	if (scale >= 20.f)
	{
		OnShockwaveFinished();
	}
	else
	{
		scale += deltaTime * 40.f;
		distanceAlpha += deltaTime * 2.f;

		if (scale >= 10.f)
		{
			bFadeDown = true;
		}

		if (bFadeDown)
		{
			fade -= deltaTime * 0.2f;
			//UE_LOG(LogTemp, Warning, TEXT("down"), );
		}
		else
		{
			fade += deltaTime * 0.2f;
			//UE_LOG(LogTemp, Warning, TEXT("up"), );
		}

		UpdatePerameters(1.f - fade, scale, distanceAlpha);
	}
}

void ACPP_Shockwave::ReverseShockwave(float deltaTime)
{
	if (scale >= 20.f)
	{
		OnShockwaveFinished();
	}
	else
	{
		scale += deltaTime * 40.f;
		distanceAlpha += deltaTime * 2.f;

		if (fade >= 0.1)
		{
			bFadeDown = true;
		}

		if (bFadeDown)
		{
			fade -= deltaTime * 0.2f;
		}
		else
		{
			fade += deltaTime * 0.2f;
		}

		UpdatePerameters(1.f - fade, 20.f - scale, 1.f - distanceAlpha);
	}
}

void ACPP_Shockwave::UpdatePerameters(float Fade, float Scale, float DistanceAlpha)
{
	Shockwave->SetScalarParameterValueOnMaterials(FName("ShockWaveFade"), Fade);
	OwnerShock->SetActorScale3D(FVector(Scale, Scale, Scale));

	FVector newLocation = FMath::Lerp(ShockWaveStartLocation, ShockWaveEndLocation, DistanceAlpha);
	//OwnerShock->SetActorLocation(newLocation);
	this->SetActorLocation(newLocation);
}

void ACPP_Shockwave::OnShockwaveFinished()
{
	if (bReversingShockwave || bSpawnedReversing)
	{
		Destroy();
	}

	UE_LOG(LogTemp, Warning, TEXT("floop"), );

	//Log the actor thats about to be destroyed
	PlayerRef->ActorsDestroyed.Add(ACPP_Shockwave::StaticClass());

	//Log the time it was destroyed
	float realTimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	PlayerRef->DestroyedTime.Add(realTimeSeconds);

	//Log the Actors Transfrom
	FTransform ActorTransform;
	ActorTransform.SetLocation(ShockWaveStartLocation);
	ActorTransform.SetRotation(OwnerShock->GetActorQuat());
	ActorTransform.SetScale3D(ShockWaveEndLocation);
	PlayerRef->DestroyedTransform.Add(ActorTransform);

	Destroy();
}


