//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#include "ProjectChronosTests.h"
#include "CPP_Rottable.h"
#include "ProjectChronosTestsCharacter.h"


// Sets default values for this component's properties
UCPP_Rottable::UCPP_Rottable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/ChronosTests/AgingObjects/M_DynamicWoodRot"));
	if (FoundMaterial.Succeeded())
	{
		DynamicMat = FoundMaterial.Object;
	}
	// ...
}


// Called when the game starts
void UCPP_Rottable::BeginPlay()
{
	Super::BeginPlay();

	CastToPlayer();

	SetUpMesh();	//TODO make the material be applied automatically
}

// Called every frame
void UCPP_Rottable::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	CheckForRotting(DeltaTime);
	// ...
}

void UCPP_Rottable::CastToPlayer()
{
	PlayerActor = GetWorld()->GetFirstPlayerController()->GetPawn();

	//This is casting to the player
	PlayerRef = Cast<AProjectChronosTestsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	Owner = GetOwner();
	Owner->Tags.Add(FName("AgeableObject"));//add tag to object

	///this tests that the casting works
	/*FString TestString = FString::SanitizeFloat(PlayerRef->testCasting);
	UE_LOG(LogTemp, Warning, TEXT("The Cast Worked The Number is %s") , *TestString);*/
}

void UCPP_Rottable::SetUpMesh()
{
	///this grabs all 1st compented mesh
	TArray<UStaticMeshComponent*>Components;
	Owner->GetComponents<UStaticMeshComponent>(Components);
	OwnerMesh = Components[0];

	OwnerMesh->bGenerateOverlapEvents = true;

	MatInstance = UMaterialInstanceDynamic::Create(DynamicMat, OwnerMesh);
	OwnerMesh->SetMaterial(0, MatInstance);
}

void UCPP_Rottable::CheckForRotting(float DeltaSeconds)
{
	if (bHasDissolved) 
	{
		//UE_LOG(LogTemp, Warning, TEXT("%s was Destroyed"), *(PlayerRef->LineTraceActorHit->GetName()));
		if (!bIsDissolving)
		{
			this->GetOwner()->Destroy();
		}
	}
	else
	{
		if (!bHasRotted)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Trace Hiting a Rottable Object"));
			if ((PlayerRef->LineTraceActorHit) == this->GetOwner())
			{
				RotObject(DeltaSeconds);
			}
		}
		else
		{
			DissolveObject(DeltaSeconds);//TODO remove this out of the line trace if loop and make it not crash 
		}
	}
}

void UCPP_Rottable::RotObject(float DeltaSeconds)
{
	//start rotting
	if (PlayerRef->bAgeableInteract)
	{
		startTime = PlayerRef->interactionStartTime;
		bIsRotting = true;

		float realTimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());
		if (perameterValue >= 1.f)
		{
			perameterValue = 1.f;
			bHasRotted = true;

			OwnerMesh->SetSimulatePhysics(true);//Make the object fall

			startTime = realTimeSeconds;

			Owner->Tags.Remove(FName("AgeableObject"));//Remove tag to object
		}
		else
		{
			perameterValue += DeltaSeconds/3;
		}

		MatInstance->SetScalarParameterValue(FName("Blend"), perameterValue);
	}
}

void UCPP_Rottable::DissolveObject(float DeltaSeconds)
{
	//start disolving
	bIsDissolving = true;

	if (perameterValue <= 0.f)
	{
		perameterValue = 0.f;
		bHasDissolved = true;
		bIsDissolving = false;
		Owner->Tags.Remove(FName("AgeableObject"));
		//UE_LOG(LogTemp, Warning, TEXT("Object has Fully Dissolved"));
	}
	else
	{
		perameterValue -= DeltaSeconds/3;
		MatInstance->SetScalarParameterValue(FName("Dissolve"), perameterValue);
		//UE_LOG(LogTemp, Warning, TEXT("Object is Dissolving"));
	}
}

