//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#include "ProjectChronosTests.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_Door.h"


// Sets default values
ACPP_Door::ACPP_Door()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetUpMesh();

	this->Tags.Add(FName("door"));
	this->Tags.Add(FName("SingularityExempt"));
}

// Called when the game starts or when spawned
void ACPP_Door::BeginPlay()
{
	Super::BeginPlay();

	CastToPlayer();

	DoorRotation = SM_Door->GetComponentQuat();
}

// Called every frame
void ACPP_Door::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bCanLerp)
	{
		lerpDoor(DeltaTime);
	}
}

void ACPP_Door::CastToPlayer()
{
	PlayerActorDoor = GetWorld()->GetFirstPlayerController()->GetPawn();

	//This is casting to the player
	PlayerRefDoor = Cast<AProjectChronosTestsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	///this tests that the casting works
	//FString TestString = FString::SanitizeFloat(PlayerRefDoor->testCasting);
	//UE_LOG(LogTemp, Warning, TEXT("The Cast Worked The Number is %s") , *TestString);
}

void ACPP_Door::SetUpMesh()
{
	struct FConstructorStatics {
		ConstructorHelpers::FObjectFinder<UStaticMesh>DoorMesh;
		ConstructorHelpers::FObjectFinder<UMaterial>DoorMat;
		FConstructorStatics()
			: DoorMesh(TEXT("/Game/StarterContent/Props/SM_Door"))
			, DoorMat(TEXT("/Game/ChronosTests/FreezingObjects/M_GlassGhost"))
		{}
	};
	static FConstructorStatics ConstructorStatics;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	SM_Door = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Door"));
	SM_Door->SetStaticMesh(ConstructorStatics.DoorMesh.Object);
	SM_Door->SetupAttachment(RootComponent);
	//SM_Door->SetMaterial(0, ConstructorStatics.DoorMat.Object);
}

void ACPP_Door::OpenCloseDoor()
{
		if (!bIsDoorOpen)		//Open the door
		{
			//FVector directionVector = PlayerRefDoor->GetActorLocation() - GetOwner()->GetActorLocation(); //potential error

			//directionVector.Normalize();

			//float dotProduct = FVector::DotProduct(directionVector, GetOwner()->GetActorForwardVector());

			//forwardsBackwards = 1.f;
			//if (dotProduct >= 0.f)
			//{
			//	forwardsBackwards = -1.f;
			//}
			LerpStart = SM_Door->GetComponentRotation();
			LerpEnd = LerpStart;

			LerpEnd.Yaw += 90.f;

			lerpStartTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());

			bIsDoorOpen = true;
			bCanLerp = true;
			//OpenDoor();
		}
		else //close the door
		{
			LerpStart = SM_Door->GetComponentRotation();
			LerpEnd = LerpStart;

			LerpEnd.Yaw -= 90.f;

			lerpStartTime = UGameplayStatics::GetRealTimeSeconds(GetWorld());

			bIsDoorOpen = false;
			bCanLerp = true;
			//CloseDoor();
		}
}

void ACPP_Door::lerpDoor(float DeltaSeconds)
{
	//UE_LOG(LogTemp, Warning, TEXT("lerping"), );
	//float now = UGameplayStatics::GetRealTimeSeconds(GetWorld());

	LerpAlpha += DeltaSeconds * 5;

	if (LerpAlpha > 1.f)
	{
		LerpAlpha = 1.f;
		bCanLerp = false;
	}

	FRotator newRotation = FMath::Lerp(LerpStart, LerpEnd, LerpAlpha);
	SM_Door->SetWorldRotation(newRotation);
}

