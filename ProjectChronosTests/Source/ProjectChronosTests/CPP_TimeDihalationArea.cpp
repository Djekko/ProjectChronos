//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#include "ProjectChronosTests.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_TimeDihalationArea.h"


// Sets default values
ACPP_TimeDihalationArea::ACPP_TimeDihalationArea()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetUpMesh();

	TimeVolume->ComponentTags.Add(FName("SingularityExempt"));

	this->SetActorScale3D(FVector(0.f, 0.f, 0.f));
}

// Called when the game starts or when spawned
void ACPP_TimeDihalationArea::BeginPlay()
{
	Super::BeginPlay();
	
	CastToPlayer();
	SetUp();
}

// Called every frame
void ACPP_TimeDihalationArea::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if ((bSpawnedReversing && !PlayerRef->bReverseButtonPressed)
		|| (!bSpawnedReversing && PlayerRef->bReverseButtonPressed))
	{
		bReversingFreezeVolume = true;
		ReverseExpand(DeltaTime);
	}
	else 
	{
		bReversingFreezeVolume = false;
		Expand(DeltaTime);
	}
}

void ACPP_TimeDihalationArea::SetUpMesh()
{
	struct FConstructorStatics {
		ConstructorHelpers::FObjectFinder<UStaticMesh>Cylinder;
		ConstructorHelpers::FObjectFinder<UMaterial>AreaMat;
		FConstructorStatics()
			: Cylinder(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder"))
			, AreaMat(TEXT("/Game/ChronosTests/FreezingObjects/M_GlassGhost"))
		{}
	};
	static FConstructorStatics ConstructorStatics;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	TimeVolume = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TimeVolume"));
	TimeVolume->SetStaticMesh(ConstructorStatics.Cylinder.Object);
	TimeVolume->SetMaterial(0, ConstructorStatics.AreaMat.Object);
	TimeVolume->SetWorldScale3D(FVector(5.25f, 5.25f, 2.35f));
	TimeVolume->SetCollisionProfileName(TEXT("OverlapAll"));
	TimeVolume->SetupAttachment(RootComponent);
	TimeVolume->OnComponentBeginOverlap.AddDynamic(this, &ACPP_TimeDihalationArea::OnOverlapBegin);
	TimeVolume->OnComponentEndOverlap.AddDynamic(this, &ACPP_TimeDihalationArea::OnEndOverlap);
}

void ACPP_TimeDihalationArea::CastToPlayer()
{
	//This is casting to the player
	PlayerRef = Cast<AProjectChronosTestsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	///this tests that the casting works
	FString TestString = FString::SanitizeFloat(PlayerRef->testCasting);
	//UE_LOG(LogTemp, Warning, TEXT("The Cast Worked from reversable The Number is %s"), *TestString);
}

void ACPP_TimeDihalationArea::SetUp()
{
	OwnerTDA = this;

	TArray<UStaticMeshComponent*>Components;
	OwnerTDA->GetComponents<UStaticMeshComponent>(Components);
	OwnerMeshTDA = Components[0];
}

void ACPP_TimeDihalationArea::Expand(float deltaSeconds)
{
	totalTime += deltaSeconds;

	if (totalTime < 0.25f)
	{
		scale += deltaSeconds * 4.0f;
	}
	else if (totalTime < 3.25f)
	{

	}
	else if (totalTime < 3.5f)
	{
		scale -= deltaSeconds * 4.0f;
	}
	else
	{
		StoreValuesForRewindingTime();
	}
	SetVolumeScale(scale);
}

void ACPP_TimeDihalationArea::ReverseExpand(float deltaSeconds)
{
	totalTime += deltaSeconds;

	if (totalTime < 0.25f)
	{
		scale += deltaSeconds * 4.0f;
	}
	else if (totalTime < 3.25f)
	{

	}
	else if (totalTime < 3.5f)
	{
		scale -= deltaSeconds * 4.0f;
	}
	else
	{
		StoreValuesForRewindingTime();
	}
	SetVolumeScale(scale);
}

void ACPP_TimeDihalationArea::SetVolumeScale(float Scale)
{
	FVector newScale = FVector(Scale, Scale, Scale);
	OwnerTDA->SetActorScale3D(newScale);
}

void ACPP_TimeDihalationArea::StoreValuesForRewindingTime()
{
	if (bReversingFreezeVolume || bSpawnedReversing)
	{
		Destroy();
	}

	//Log the actor thats about to be destroyed
	PlayerRef->ActorsDestroyed.Add(ACPP_TimeDihalationArea::StaticClass());

	//Log the time it was destroyed
	float realTimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	PlayerRef->DestroyedTime.Add(realTimeSeconds);

	//Log the Actors Transfrom
	FTransform ActorTransform = OwnerTDA->GetTransform();
	PlayerRef->DestroyedTransform.Add(ActorTransform);

	Destroy();
}

void ACPP_TimeDihalationArea::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	OtherActor->CustomTimeDilation = 0.05f;

	if (OtherActor == PlayerRef)
	{
		PlayerRef->bPlayerInTimeVolume = true;

		FakeSlowDilation(OtherComp, true);

		///Sort the time dilation for all actors involved
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 20.f);
		OwnerTDA->CustomTimeDilation = 0.05f;

		TArray<UPrimitiveComponent*> OverlappingComponents;
		TimeVolume->GetOverlappingComponents(OverlappingComponents);

		for (int x = 0; x < OverlappingComponents.Num(); x++)
		{
			OverlappingComponents[x]->SetEnableGravity(true);
		}

		if (OtherComp->ComponentTags.Contains(FName("IsTimeDilated")))
		{
			OtherComp->ComponentTags.Remove(FName("IsTimeDilated"));
		}
	}
	else 
	{
		FakeSlowDilation(OtherComp, false);
	}
}

void ACPP_TimeDihalationArea::OnEndOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	FakeNormalDilation(OtherComp);

	OtherActor->CustomTimeDilation = 1.f;

	if (OtherActor == PlayerRef)
	{
		PlayerRef->bPlayerInTimeVolume = false;

		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.f);

		OwnerTDA->CustomTimeDilation = 1.f;

		TArray<UPrimitiveComponent*> OverlappingComponents;
		TimeVolume->GetOverlappingComponents(OverlappingComponents);

		for (int x = 0; x < OverlappingComponents.Num(); x++)
		{
			OverlappingComponents[x]->SetEnableGravity(false);
			if (OverlappingComponents[x]->ComponentTags.Contains(FName("IsTimeDilated")))
			{
				OverlappingComponents[x]->ComponentTags.Remove(FName("IsTimeDilated"));
			}
		}
	}
}

void ACPP_TimeDihalationArea::FakeSlowDilation(UPrimitiveComponent * TargetComp, bool bEnablegravity)
{
	TargetComp->ComponentTags.Add(FName("IsTimeDilated"));

	TargetComp->SetEnableGravity(bEnablegravity);
	FVector newLinearVelocity = TargetComp->GetPhysicsLinearVelocity() / 20.f;
	FVector newAngularVelocity = TargetComp->GetPhysicsAngularVelocity() / 20.f;
	TargetComp->SetPhysicsLinearVelocity(newLinearVelocity);
	TargetComp->SetPhysicsAngularVelocity(newAngularVelocity);
}

void ACPP_TimeDihalationArea::FakeNormalDilation(UPrimitiveComponent * TargetComp)
{
	TargetComp->SetEnableGravity(true);
	FVector newLinearVelocity = TargetComp->GetPhysicsLinearVelocity() * 5.f;
	FVector newAngularVelocity = TargetComp->GetPhysicsAngularVelocity() * 5.f;
	TargetComp->SetPhysicsLinearVelocity(newLinearVelocity);
	TargetComp->SetPhysicsAngularVelocity(newAngularVelocity);
}

void ACPP_TimeDihalationArea::TickFunction()
{
	TArray<AActor*> OverlappingActors;
	TimeVolume->GetOverlappingActors(OverlappingActors);

	for (int x = 0; x < OverlappingActors.Num(); x++)
	{
		bool bIsExempt = OverlappingActors[x]->Tags.Contains(FName("TimeExempt"));
		if ((OverlappingActors[x] != OwnerTDA) && !bIsExempt) 
		{
			OverlappingActors[x]->CustomTimeDilation = timeDilation;
		}
	}
}


