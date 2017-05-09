//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#include "ProjectChronosTests.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_Ladder.h"
#include "CPP_LadderTop.h"


// Sets default values
ACPP_LadderTop::ACPP_LadderTop()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetUpMesh();

	SM_LadderTop->ComponentTags.Add(FName("SingularityExempt"));
	SM_LadderSide->ComponentTags.Add(FName("SingularityExempt"));
	MainBox->ComponentTags.Add(FName("SingularityExempt"));
	BackupBox->ComponentTags.Add(FName("SingularityExempt"));
	connectionArrow->ComponentTags.Add(FName("SingularityExempt"));
	directionArrow->ComponentTags.Add(FName("SingularityExempt"));
}

// Called when the game starts or when spawned
void ACPP_LadderTop::BeginPlay()
{
	Super::BeginPlay();
	
	CastToPlayer();

	FVector Location = connectionArrow->GetComponentLocation();
	FRotator Rotation = SM_LadderSide->GetComponentRotation();
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	GetWorld()->SpawnActor<ACPP_Ladder>
		( Location
			, Rotation
			, SpawnInfo
			);
}

// Called every frame
void ACPP_LadderTop::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (bCanLerp) {
		LerpPlayer(DeltaTime);
	}
}

void ACPP_LadderTop::CastToPlayer()
{
	PlayerActorLadderTop = GetWorld()->GetFirstPlayerController()->GetPawn();

	//This is casting to the player
	PlayerRefLadderTop = Cast<AProjectChronosTestsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	///this tests that the casting works
	//FString TestString = FString::SanitizeFloat(PlayerRefDoor->testCasting);
	//UE_LOG(LogTemp, Warning, TEXT("The Cast Worked The Number is %s") , *TestString);
}

void ACPP_LadderTop::SetUpMesh()
{
	struct FConstructorStatics {
		ConstructorHelpers::FObjectFinder<UStaticMesh>LadderMeshTop;
		ConstructorHelpers::FObjectFinder<UStaticMesh>LadderMeshBody;
		ConstructorHelpers::FObjectFinder<UMaterial>LadderMat;
		ConstructorHelpers::FObjectFinder<UStaticMesh>Arrowmesh;
		ConstructorHelpers::FObjectFinder<UMaterial>ArrowMat1;
		ConstructorHelpers::FObjectFinder<UMaterial>ArrowMat2;
		FConstructorStatics()
			: LadderMeshTop(TEXT("/Game/ChronosTests/3rdPartyAssets/aaronAssets/Ladder_Top"))
			, LadderMeshBody(TEXT("/Game/ChronosTests/3rdPartyAssets/aaronAssets/LadderMain"))
			, LadderMat(TEXT("/Game/StarterContent/Materials/M_Metal_Burnished_Steel"))
			, Arrowmesh(TEXT("/Game/StarterContent/Shapes/Shape_Cone"))
			, ArrowMat1(TEXT("/Game/StarterContent/Materials/M_ArrowMat"))
			, ArrowMat2(TEXT("/Game/StarterContent/Materials/M_ArrowMat2"))
		{}
	};
	static FConstructorStatics ConstructorStatics;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	SM_LadderTop = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_LadderTop"));
	SM_LadderTop->SetStaticMesh(ConstructorStatics.LadderMeshTop.Object);
	SM_LadderTop->SetMaterial(0, ConstructorStatics.LadderMat.Object);
	SM_LadderTop->SetRelativeLocation(FVector(0.f,19.f,-1.051506f));
	SM_LadderTop->SetupAttachment(RootComponent);

	SM_LadderSide = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_LadderSide"));
	SM_LadderSide->SetStaticMesh(ConstructorStatics.LadderMeshBody.Object);
	SM_LadderSide->SetMaterial(0, ConstructorStatics.LadderMat.Object);
	SM_LadderSide->SetRelativeLocation(FVector(0.f, 19.f, -101.072739f));
	SM_LadderSide->SetupAttachment(RootComponent);

	MainBox = CreateDefaultSubobject<UBoxComponent>(TEXT("MainBox"));
	MainBox->SetRelativeLocation(FVector(0.f, 85.f, 200.f));
	MainBox->SetRelativeScale3D(FVector(3.051758f, 1.953125f, 0.421875f));
	MainBox->SetupAttachment(RootComponent);
	MainBox->OnComponentBeginOverlap.AddDynamic(this, &ACPP_LadderTop::OnOverlapBegin);

	BackupBox = CreateDefaultSubobject<UBoxComponent>(TEXT("BackupBox"));
	BackupBox->SetRelativeLocation(FVector(0.f, 85.f, 300.f));
	BackupBox->SetRelativeScale3D(FVector(3.051758f, 1.953125f, 0.421875f));
	BackupBox->SetupAttachment(RootComponent);
	BackupBox->OnComponentBeginOverlap.AddDynamic(this, &ACPP_LadderTop::OnOverlapBegin);

	connectionArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("connectionArrow"));
	connectionArrow->SetStaticMesh(ConstructorStatics.Arrowmesh.Object);
	connectionArrow->SetMaterial(0, ConstructorStatics.ArrowMat1.Object);
	connectionArrow->SetRelativeLocation(FVector(0.f, 36.834641f, -101.075447f));
	connectionArrow->SetRelativeScale3D(FVector(0.15f, 0.15f, 1.f));
	connectionArrow->SetRelativeRotation(FRotator(180.f, 0.f, 0.f));
	connectionArrow->SetupAttachment(RootComponent);
	connectionArrow->bHiddenInGame = true;
	connectionArrow->SetCollisionProfileName(TEXT("OverlapAll"));

	directionArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("directionArrow"));
	directionArrow->SetStaticMesh(ConstructorStatics.Arrowmesh.Object);
	directionArrow->SetMaterial(0, ConstructorStatics.ArrowMat2.Object);
	directionArrow->SetRelativeLocation(FVector(0.f, 78.f, -76.f));
	directionArrow->SetRelativeScale3D(FVector(0.15f, 0.15f, 1.f));
	directionArrow->SetupAttachment(RootComponent);
	directionArrow->bHiddenInGame = true;
	directionArrow->SetCollisionProfileName(TEXT("OverlapAll"));
}

void ACPP_LadderTop::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (!OtherActor->Tags.Contains(FName("Singularity")))
	{
		if (PlayerRefLadderTop->bCanClimbLadder)
		{
			PlayerRefLadderTop->GetCharacterMovement()->SetMovementMode(MOVE_Walking);

			lerpStart = PlayerRefLadderTop->GetCapsuleComponent()->GetComponentLocation();
			lerpEnd = RootComponent->GetComponentLocation();
			lerpEnd.Z = lerpStart.Z;
			alpha = 0.f;
			bCanLerp = true;
		}
		else
		{
			PlayerRefLadderTop->GetCharacterMovement()->SetMovementMode(MOVE_Flying);

			lerpStart = PlayerRefLadderTop->GetCapsuleComponent()->GetComponentLocation();
			lerpEnd = directionArrow->GetComponentLocation();
			lerpEnd.Z = lerpStart.Z;
			alpha = 0.f;
			bCanLerp = true;
		}
	}
}

void ACPP_LadderTop::LerpPlayer(float DeltaSeconds)
{
	alpha += DeltaSeconds * 2;

	if (alpha > 1.f)
	{
		alpha = 1.f;
		bCanLerp = false;

		PlayerRefLadderTop->bCanClimbLadder = !PlayerRefLadderTop->bCanClimbLadder;
	}

	FVector newPos = FMath::Lerp(lerpStart, lerpEnd, alpha);

	PlayerRefLadderTop->GetCapsuleComponent()->SetWorldLocation(newPos);
}



