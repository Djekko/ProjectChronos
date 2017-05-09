//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#include "ProjectChronosTests.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_TimeDihalationAreaGhost.h"


// Sets default values
ACPP_TimeDihalationAreaGhost::ACPP_TimeDihalationAreaGhost()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetUpMesh();

	TimeVolume->ComponentTags.Add(FName("SingularityExempt"));
}

// Called when the game starts or when spawned
void ACPP_TimeDihalationAreaGhost::BeginPlay()
{
	Super::BeginPlay();

	CastToPlayer();
	SetUp();
}

// Called every frame
void ACPP_TimeDihalationAreaGhost::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	UpdateGhostInfrontOfPlayer();
}

void ACPP_TimeDihalationAreaGhost::SetUpMesh()
{
	struct FConstructorStatics {
		ConstructorHelpers::FObjectFinder<UStaticMesh>Cylinder;
		ConstructorHelpers::FObjectFinder<UMaterial>AreaMat;
		FConstructorStatics()
			: Cylinder(TEXT("/Game/StarterContent/Shapes/Shape_Cylinder"))
			, AreaMat(TEXT("/Game/ChronosTests/FreezingObjects/M_GlassGhostGreen"))
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
}

void ACPP_TimeDihalationAreaGhost::CastToPlayer()
{
	//This is casting to the player
	PlayerRef = Cast<AProjectChronosTestsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	///this tests that the casting works
	FString TestString = FString::SanitizeFloat(PlayerRef->testCasting);
	//UE_LOG(LogTemp, Warning, TEXT("The Cast Worked from reversable The Number is %s"), *TestString);
}

void ACPP_TimeDihalationAreaGhost::SetUp()
{
	OwnerTDAG = this;

	TArray<UStaticMeshComponent*>Components;
	OwnerTDAG->GetComponents<UStaticMeshComponent>(Components);
	OwnerMeshTDAG = Components[0];
}

void ACPP_TimeDihalationAreaGhost::UpdateGhostInfrontOfPlayer()
{
	Root->SetWorldLocation(NewGhostPos());
}

FVector ACPP_TimeDihalationAreaGhost::NewGhostPos()
{
	FVector TempLocation = PlayerRef->GetFirstPersonCameraComponent()->GetComponentLocation();
	FQuat TempRotation = PlayerRef->GetFirstPersonCameraComponent()->GetComponentQuat();

	FVector forwardVector = TempRotation.GetForwardVector();
	forwardVector = forwardVector * PlayerRef->dihalationAreaDistance;

	FVector result = TempLocation + forwardVector;

	return result;
}

