//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#include "ProjectChronosTests.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_ReversableMovingObj.h"


// Sets default values for this component's properties
UCPP_ReversableMovingObj::UCPP_ReversableMovingObj()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UCPP_ReversableMovingObj::BeginPlay()
{
	Super::BeginPlay();

	CastToPlayer();

	SetUp();
}


// Called every frame
void UCPP_ReversableMovingObj::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	ProcessTransformsAndVelocities();
}

void UCPP_ReversableMovingObj::CastToPlayer()
{
	//This is casting to the player
	ReversablePlayerRef = Cast<AProjectChronosTestsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	///this tests that the casting works
	FString TestString = FString::SanitizeFloat(ReversablePlayerRef->testCasting);
	//UE_LOG(LogTemp, Warning, TEXT("The Cast Worked from reversable The Number is %s"), *TestString);
}

void UCPP_ReversableMovingObj::SetUp()
{
	Owner = GetOwner();
	Owner->Tags.Add(FName("ReveraibleObject"));//add tag to object

	TArray<UStaticMeshComponent*>Components;
	Owner->GetComponents<UStaticMeshComponent>(Components);
	OwnerMesh = Components[0];

	OwnerMesh->bGenerateOverlapEvents = true;

	OwnerMesh->SetSimulatePhysics(true);
	if (bIsStaticObject)
	{
		OwnerMesh->SetSimulatePhysics(false);
	}
}

void UCPP_ReversableMovingObj::ProcessTransformsAndVelocities()
{
	if (ReversablePlayerRef->bReverseButtonPressed)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Reverse"));
		ReverseTransforms();
		if (!bIsStaticObject) 
		{
			ReverseVelocities();
		}
	}
	else 
	{
		StoreTransforms();
		if (!bIsStaticObject)
		{
			StoreVelocities();
		}
	}
}

void UCPP_ReversableMovingObj::ReverseTransforms()
{
	//UE_LOG(LogTemp, Warning, TEXT("Reversing Transform"));

	int32 lastIndex = TransformArray.Num() - 1;
	FTransform NewTrans = TransformArray[lastIndex];
	OwnerMesh->SetWorldTransform(NewTrans);

	
	if (lastIndex > 0)
	{
		ReplayTransform.Add(NewTrans);
		TransformArray.RemoveAt(lastIndex);
	}
}

void UCPP_ReversableMovingObj::StoreTransforms()
{
	//UE_LOG(LogTemp, Warning, TEXT("Storing Transform"));
	if (TransformArray.Num() >= maxArrayLength)
	{
		TransformArray.RemoveAt(0);
	}
	
	TransformArray.Add(OwnerMesh->GetComponentTransform());
	//UE_LOG(LogTemp, Warning, TEXT("%d Total array elements"), TransformArray.Num());

	ReplayTransforms();
}

void UCPP_ReversableMovingObj::ReplayTransforms()
{
	//UE_LOG(LogTemp, Warning, TEXT("Replaying Transform"));
	if (!Owner->Tags.Contains("CanBreak"))
	{
		Owner->Tags.Add("CanBreak");
	}

	if (Owner->Tags.Contains("BreakTimeReplay"))
	{
		TArray<FTransform> emptyArray;
		ReplayTransform = emptyArray;
	}
	else
	{
		int32 lastIndex = ReplayTransform.Num() - 1;
		if (lastIndex > 0)
		{
			FTransform newTransform = ReplayTransform[lastIndex];
			OwnerMesh->SetWorldTransform(newTransform);
			
			ReplayTransform.RemoveAt(lastIndex);
		}
		else
		{
			if (Owner->Tags.Contains("CanBreak"))
			{
				Owner->Tags.Remove("CanBreak");
			}
		}
	}
}

void UCPP_ReversableMovingObj::ReverseVelocities()
{
	bResetVelocity = true;

	int32 lastIndex = VelocityArray.Num() - 1;
	FTransform lastVelocityTrans = VelocityArray[lastIndex];
	
	FVector newLinVelocity = lastVelocityTrans.GetLocation() * -1.f;
	FVector newAngVelocity = lastVelocityTrans.GetScale3D() * -1.f;
	OwnerMesh->SetPhysicsLinearVelocity(newLinVelocity);
	OwnerMesh->SetPhysicsAngularVelocity(newAngVelocity);

	if (lastIndex > 0)
	{
		ReplayVelocity.Add(lastVelocityTrans);
		VelocityArray.RemoveAt(lastIndex);
	}
}

void UCPP_ReversableMovingObj::StoreVelocities()
{
	if (bResetVelocity)
	{
		FVector newLinVelocity = OwnerMesh->GetPhysicsLinearVelocity() * -1.f;
		FVector newAngVelocity = OwnerMesh->GetPhysicsAngularVelocity() * -1.f;
		OwnerMesh->SetPhysicsLinearVelocity(newLinVelocity);
		OwnerMesh->SetPhysicsAngularVelocity(newAngVelocity);

		bResetVelocity = false;
	}

	int32 lastIndex = VelocityArray.Num() - 1;
	if (lastIndex >= maxArrayLength)
	{
		VelocityArray.RemoveAt(0);
	}

	FVector newLinear = OwnerMesh->GetPhysicsLinearVelocity();
	FQuat newRotation = (FQuat(0.f, 0.f, 0.f, 0.f));
	FVector newAngular = OwnerMesh->GetPhysicsAngularVelocity();

	FTransform newTransformVelocity;
	newTransformVelocity.SetLocation(newLinear);
	newTransformVelocity.SetRotation(newRotation);
	newTransformVelocity.SetScale3D(newAngular);

	VelocityArray.Add(newTransformVelocity);

	ReplayVelocities();
}

void UCPP_ReversableMovingObj::ReplayVelocities()
{
	if (Owner->Tags.Contains("BreakTimeReplay"))
	{
		TArray<FTransform> emptyArray;
		ReplayVelocity = emptyArray;
	}
	else 
	{
		int32 lastIndex = ReplayVelocity.Num() - 1;
		if (lastIndex > 0)
		{
			FVector newLinear = ReplayVelocity[lastIndex].GetLocation();
			FVector newAngular = ReplayVelocity[lastIndex].GetScale3D();
			OwnerMesh->SetPhysicsLinearVelocity(newLinear);
			OwnerMesh->SetPhysicsAngularVelocity(newAngular);

			ReplayVelocity.RemoveAt(lastIndex);
		}
	}
}

