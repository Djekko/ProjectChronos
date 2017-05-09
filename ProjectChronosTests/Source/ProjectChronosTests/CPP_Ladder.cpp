//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#include "ProjectChronosTests.h"
#include "ProjectChronosTestsCharacter.h"
#include "Components/ArrowComponent.h"
#include "CPP_Ladder.h"


#define OUT

// Sets default values
ACPP_Ladder::ACPP_Ladder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetUpMesh();

	SM_Ladder->ComponentTags.Add(FName("Ladder"));
	SM_Ladder->ComponentTags.Add(FName("SingularityExempt"));
}

// Called when the game starts or when spawned
void ACPP_Ladder::BeginPlay()
{
	Super::BeginPlay();

	CastToPlayer();
	
	LineTrace();
}

// Called every frame
void ACPP_Ladder::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	if (bCanLerp)
	{
		LerpPlayer(DeltaTime);
	}
}

void ACPP_Ladder::CastToPlayer()
{
	PlayerActorLadder = GetWorld()->GetFirstPlayerController()->GetPawn();

	//This is casting to the player
	PlayerRefLadder = Cast<AProjectChronosTestsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	///this tests that the casting works
	//FString TestString = FString::SanitizeFloat(PlayerRefDoor->testCasting);
	//UE_LOG(LogTemp, Warning, TEXT("The Cast Worked The Number is %s") , *TestString);
}

void ACPP_Ladder::SetUpMesh()
{
	struct FConstructorStatics {
		ConstructorHelpers::FObjectFinder<UStaticMesh>LadderMesh;
		ConstructorHelpers::FObjectFinder<UMaterial>LadderMat;
		ConstructorHelpers::FObjectFinder<UStaticMesh>Arrowmesh;
		ConstructorHelpers::FObjectFinder<UMaterial>ArrowMat1;
		ConstructorHelpers::FObjectFinder<UMaterial>ArrowMat2;
		FConstructorStatics()
			: LadderMesh(TEXT("/Game/ChronosTests/3rdPartyAssets/aaronAssets/LadderMain"))
			, LadderMat(TEXT("/Game/StarterContent/Materials/M_Metal_Burnished_Steel"))
			, Arrowmesh(TEXT("/Game/StarterContent/Shapes/Shape_Cone"))
			, ArrowMat1(TEXT("/Game/StarterContent/Materials/M_ArrowMat"))
			, ArrowMat2(TEXT("/Game/StarterContent/Materials/M_ArrowMat2"))
		{}
	};
	static FConstructorStatics ConstructorStatics;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	SM_Ladder = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SM_Ladder"));
	SM_Ladder->SetStaticMesh(ConstructorStatics.LadderMesh.Object);
	SM_Ladder->SetMaterial(0, ConstructorStatics.LadderMat.Object);
	SM_Ladder->SetRelativeLocation(FVector(0.f, -18.f, -100.f));
	SM_Ladder->SetupAttachment(RootComponent);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	CollisionBox->SetRelativeLocation(FVector(0.f, 30.f, -50.f));
	CollisionBox->SetRelativeScale3D(FVector(0.5625f, 0.44754f, 1.464844f));
	CollisionBox->SetupAttachment(RootComponent);
	CollisionBox->OnComponentBeginOverlap.AddDynamic(this, &ACPP_Ladder::OnOverlapBegin);
	CollisionBox->bHiddenInGame = false;

	connectionArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("connectionArrow"));
	connectionArrow->SetStaticMesh(ConstructorStatics.Arrowmesh.Object);
	connectionArrow->SetMaterial(0, ConstructorStatics.ArrowMat1.Object);
	connectionArrow->SetRelativeLocation(FVector(0.f, 0.f, -100.f));
	connectionArrow->SetRelativeScale3D(FVector(0.15f, 0.15f, 1.f));
	connectionArrow->SetRelativeRotation(FRotator(180.f, 0.f, 0.f));
	connectionArrow->SetupAttachment(RootComponent);
	connectionArrow->bHiddenInGame = true;
	connectionArrow->SetCollisionProfileName(TEXT("OverlapAll"));

	directionArrow = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("directionArrow"));
	directionArrow->SetStaticMesh(ConstructorStatics.Arrowmesh.Object);
	directionArrow->SetMaterial(0, ConstructorStatics.ArrowMat2.Object);
	directionArrow->SetRelativeLocation(FVector(0.f, 57.f, -80.f));
	directionArrow->SetRelativeScale3D(FVector(0.15f, 0.15f, 1.f));
	directionArrow->SetupAttachment(RootComponent);
	directionArrow->bHiddenInGame = true;
	directionArrow->SetCollisionProfileName(TEXT("OverlapAll"));
}

void ACPP_Ladder::LineTrace()
{
	/// setup querry params
	FCollisionQueryParams TraceParams(FName(TEXT("")), false, GetOwner());

	FVector traceStart = connectionArrow->GetComponentLocation();
	traceStart.Z -= 1.f;
	FVector traceEnd = traceStart;
	traceEnd.Z = connectionArrow->GetComponentLocation().Z - 100.f;

	bool bTraceHitSomething = false;
	/// LineTrace/Ray-Cast out to reach distance
	FHitResult LineTraceHit;
	GetWorld()->LineTraceSingleByChannel(
		OUT LineTraceHit,
		traceStart,
		traceEnd,
		ECC_Visibility,
		TraceParams
	);

	/// log actor hit
	bool bBlockingHit = LineTraceHit.bBlockingHit;
	AActor* LineTraceActorHit = LineTraceHit.GetActor();
	UPrimitiveComponent* LineTraceComponentHit = LineTraceHit.GetComponent();
	if (!bBlockingHit)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Line Trace Missed ----- Building another ladder"), );

		FVector Location = connectionArrow->GetComponentLocation();
		FRotator Rotation = SM_Ladder->GetComponentRotation();
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		GetWorld()->SpawnActor<ACPP_Ladder>
			(Location
				, Rotation
				, SpawnInfo
				);

		CollisionBox->DestroyComponent();
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("Line Trace Hit: %s"), *(LineTraceActorHit->GetName()));
	}
	
}

void ACPP_Ladder::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{ 
	if (OtherComp == PlayerRefLadder->GetCapsuleComponent() && OverlappedComp == CollisionBox)
	{
		if (PlayerRefLadder->bCanClimbLadder)
		{
			PlayerRefLadder->bCanClimbLadder = false;
			PlayerRefLadder->GetCharacterMovement()->SetMovementMode(MOVE_Walking, 0);
		}
		else
		{
			if (PlayerRefLadder->bIsFacingLadder)
			{
			PlayerRefLadder->bCanClimbLadder = true;
			PlayerRefLadder->GetCharacterMovement()->SetMovementMode(MOVE_Flying, 0);
			PlayerRefLadder->GetCharacterMovement()->Velocity = FVector(0.f, 0.f, 0.f);

			LerpStartPos = PlayerRefLadder->GetCapsuleComponent()->GetComponentLocation();
			LerpEndPos = directionArrow->GetComponentLocation();
			LerpEndPos.Z = LerpStartPos.Z;

			LerpStartQuat = PlayerRefLadder->GetCapsuleComponent()->GetComponentQuat();
			LerpEndQuat = directionArrow->GetComponentQuat();
			LerpEndQuat.Z = LerpStartQuat.Z;

			alpha = 0.f;
			bCanLerp = true;
			}
		}
	}
}

void ACPP_Ladder::LerpPlayer(float DeltaSeconds)
{
	//UE_LOG(LogTemp, Warning, TEXT("tis but a lerp"), );
	//float now = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	alpha += DeltaSeconds * 2;

	if (alpha > 1.f)
	{
		alpha = 1.f;
		bCanLerp = false;
	}

	FVector newPos = FMath::Lerp(LerpStartPos, LerpEndPos, alpha);
	PlayerRefLadder->GetCapsuleComponent()->SetWorldLocation(newPos);

	//TODO fix so the player rotates to the ladder
	/*FQuat newQuat = FMath::Lerp(LerpStartQuat, LerpEndQuat, alpha);
	PlayerRefLadder->GetCapsuleComponent()->SetWorldRotation(newQuat);*/
}

