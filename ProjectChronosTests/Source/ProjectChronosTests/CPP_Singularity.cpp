//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#include "ProjectChronosTests.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_Singularity.h"


// Sets default values
ACPP_Singularity::ACPP_Singularity()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterial> FoundMaterial(TEXT("/Game/ChronosTests/BlackHole/VFX/Materials/M_VolumeSphereMaster2"));
	if (FoundMaterial.Succeeded())
	{
		ShaderMaterial = FoundMaterial.Object;
	}

	SetUpMesh();

	this->SetActorScale3D(FVector(0.f, 0.f, 0.f));
}

// Called when the game starts or when spawned
void ACPP_Singularity::BeginPlay()
{
	Super::BeginPlay();

	SingularityOwner = this;
	SingularityOwner->Tags.Add(FName("TimeExempt"));//add tag to object
	SingularityOwner->Tags.Add(FName("Singularity"));//add tag to object

	CastToPlayer();
}

// Called every frame
void ACPP_Singularity::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

	TickFunction();

	if ((bSpawnedRevering && !SingularityPlayerRef->bReverseButtonPressed)
		|| (!bSpawnedRevering && SingularityPlayerRef->bReverseButtonPressed))
	{
		bReversingSingularity = true;

		ReverseSingularity(DeltaTime);
	}
	else
	{
		bReversingSingularity = false;

		ExpandSingularity(DeltaTime);
	}
}

void ACPP_Singularity::SetUpMesh()
{
	struct FConstructorStatics 
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh>BH;
		ConstructorHelpers::FObjectFinder<UMaterial>BlackHoleMat;
		ConstructorHelpers::FObjectFinder<UStaticMesh>AoE;
		ConstructorHelpers::FObjectFinder<UMaterial>AoEMat;
		ConstructorHelpers::FObjectFinder<UParticleSystem>BlackHoleParts;
		FConstructorStatics()
			: BH(TEXT("/Game/StarterContent/Props/MaterialSphere"))
			, BlackHoleMat(TEXT("/Game/ChronosTests/BlackHole/VFX/Materials/BlackHole"))
			, AoE(TEXT("/Game/StarterContent/Props/MaterialSphere"))
			, AoEMat(TEXT("/Game/StarterContent/Materials/M_Glass"))
			, BlackHoleParts(TEXT("/Game/ChronosTests/BlackHole/VFX/P_SingularityTest"))
		{}
	};
	static FConstructorStatics ConstructorStatics;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = Root;

	///BlackHole
	BlackHole = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlackHole"));
	BlackHole->SetStaticMesh(ConstructorStatics.BH.Object);
	BlackHole->SetMaterial(0, ConstructorStatics.BlackHoleMat.Object);
	BlackHole->SetCollisionProfileName(TEXT("OverlapAll"));
	BlackHole->ComponentTags.Add(FName("TimeExempt"));
	BlackHole->ComponentTags.Add(FName("SingularityExempt"));
	BlackHole->SetupAttachment(RootComponent);
	BlackHole->OnComponentBeginOverlap.AddDynamic(this, &ACPP_Singularity::OnOverlapBegin);

	///Area Of Effect
	AreaOfEffect = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AreaOfEffect"));
	AreaOfEffect->SetStaticMesh(ConstructorStatics.AoE.Object);
	AreaOfEffect->SetMaterial(0, ConstructorStatics.AoEMat.Object);
	AreaOfEffect->SetRelativeScale3D(FVector(6.9f, 6.9f, 6.9f));
	AreaOfEffect->bHiddenInGame = true;
	AreaOfEffect->SetCollisionProfileName(TEXT("OverlapAll"));
	AreaOfEffect->SetupAttachment(RootComponent);
	AreaOfEffect->OnComponentBeginOverlap.AddDynamic(this, &ACPP_Singularity::OnOverlapBegin);
	AreaOfEffect->OnComponentEndOverlap.AddDynamic(this, &ACPP_Singularity::OnEndOverlap);


	///Particle System
	SingularityParticles = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SingularityParticles"));
	
	SingularityParticles->Template = ConstructorStatics.BlackHoleParts.Object;
	SingularityParticles->SetupAttachment(RootComponent);
}

void ACPP_Singularity::CastToPlayer()
{
	SingularityPlayerActor = GetWorld()->GetFirstPlayerController()->GetPawn();

	//This is casting to the player
	SingularityPlayerRef = Cast<AProjectChronosTestsCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	///this tests that the casting works
	FString TestString = FString::SanitizeFloat(SingularityPlayerRef->testCasting);
	//UE_LOG(LogTemp, Warning, TEXT("The Cast Worked The Number is %s") , *TestString);

	if (SingularityPlayerRef->bReverseButtonPressed)
	{
		bSpawnedRevering = true;
	}
	else
	{
		bSpawnedRevering = false;
	}
}

void ACPP_Singularity::ExpandSingularity(float deltaSeconds)
{
	totalTime += deltaSeconds;

	if (totalTime < 0.25f)
	{
		scale += deltaSeconds * 4.0f;
	}
	else if (totalTime < 3.25f)
	{

	}
	else if(totalTime < 3.5f)
	{
		scale -= deltaSeconds * 4.0f;
	}
	else
	{
		FinishedExpanding();
	}
	UpdateExpanding(scale);
}

void ACPP_Singularity::ReverseSingularity(float deltaSeconds)
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
		FinishedExpanding();
	}
	UpdateExpanding(scale);
}

void ACPP_Singularity::UpdateExpanding(float Scale)
{
	FVector newScale = FVector(Scale, Scale, Scale);

	SingularityOwner->SetActorScale3D(newScale);
}

void ACPP_Singularity::FinishedExpanding()
{
	if (bReversingSingularity || bSpawnedRevering)
	{
		Destroy();
	}

	//Log the actor thats about to be destroyed
	SingularityPlayerRef->ActorsDestroyed.Add(ACPP_Singularity::StaticClass());
	//Log the time it was destroyed
	float realTimeSeconds = UGameplayStatics::GetRealTimeSeconds(GetWorld());
	SingularityPlayerRef->DestroyedTime.Add(realTimeSeconds);
	//Log the Actors Transfrom
	FTransform ActorTransform = SingularityOwner->GetTransform();
	SingularityPlayerRef->DestroyedTransform.Add(ActorTransform);

	Destroy();
}

void ACPP_Singularity::OnOverlapBegin(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	bool bSingularityExempt = OtherComp->ComponentTags.Contains(FName("SingularityExempt"));

	if (!bSingularityExempt)
	{
		//Overlap event for blackhole
		if (OverlappedComp == AreaOfEffect)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s overlapped with %s"), *OtherActor->GetName(), *AreaOfEffect->GetName());

			///if the component overlapping with the AoE isnt singularity exempt
			bSingularityExempt = OtherComp->ComponentTags.Contains(FName("SingularityExempt"));
			if (!bSingularityExempt && OtherComp != BlackHole)
			{
				///Turn off grasvity and overlap other components
				OtherComp->SetEnableGravity(false);
				OtherComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

				///Give the Comp a random rotation
				float Floats[3];
				for (int a = 0; a < 3; a++)
				{
					float RandFloat = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 360));
					Floats[a] = RandFloat - 180.f;
				}
				OtherComp->SetAllPhysicsAngularVelocity(FVector(Floats[0], Floats[1], Floats[2]));

				/*static ConstructorHelpers::FObjectFinder<UMaterial>FoundMat(TEXT("/Game/ChronosTests/BlackHole/VFX/Materials/M_VolumeSphereMaster2"));
				if (FoundMat.Succeeded())
				{
				ShaderMaterial = FoundMat.Object;
				}
				ShaderMatInst = UMaterialInstanceDynamic::Create(ShaderMaterial, OtherComp);

				OtherComp->SetMaterial(0, ShaderMatInst);*/

				ShaderMatInst = UMaterialInstanceDynamic::Create(ShaderMaterial, OtherComp);
				OtherComp->SetMaterial(0, ShaderMatInst);
				OtherComp->SetMaterial(1, ShaderMatInst);

				FVector location = BlackHole->GetComponentLocation();
				float x = location.X;
				float y = location.Y;
				float z = location.Z;
				ShaderMatInst->SetVectorParameterValue(FName("CollapseLocation"), (FLinearColor(x, y, z, 0.f)));

				OtherComp->SetPhysicsLinearVelocity(FVector(0.f, 0.f, 0.f));
			}
		}

		//Overlap Event for area of effect
		if (OverlappedComp == BlackHole)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s overlapped with %s"), *OtherActor->GetName(), *BlackHole->GetName());

			bSingularityExempt = OtherComp->ComponentTags.Contains(FName("SingularityExempt"));
			if (OtherActor != this && !bSingularityExempt)
			{
				UE_LOG(LogTemp, Warning, TEXT("%s is being destroyed"), *OtherActor->GetName());
				OtherActor->Destroy();
			}
		}
	}
	
}

void ACPP_Singularity::OnEndOverlap(UPrimitiveComponent * OverlappedComp, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex)
{
	///if the component overlapping with the AoE isnt singularity exempt
	bool bSingularityExempt = OtherComp->ComponentTags.Contains(FName("SingularityExempt"));
	if (!bSingularityExempt)
	{
		///Turn on gravity and overlap other components
		OtherComp->SetEnableGravity(true);
		OtherComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

		///make sure the scale is set back to normal
		//OtherComp->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
	}
}

void ACPP_Singularity::TickFunction()
{
	

	TArray<UPrimitiveComponent*> OverlappingComps;
	AreaOfEffect->GetOverlappingComponents(OverlappingComps);

	for (int x = 0; x < OverlappingComps.Num(); x++)
	{
		if ((OverlappingComps[x]->GetOwner() != this)
			&& (!OverlappingComps[x]->ComponentTags.Contains(FName("SingularityExempt"))))
		{
			FVector OverlappingCompLocation = OverlappingComps[x]->GetComponentLocation();
			FVector BlackHoleLocation = BlackHole->GetComponentLocation();

			//UE_LOG(LogTemp, Warning, TEXT("objects should be moving to blackhole"));

			FVector NewVelocity = (BlackHoleLocation - OverlappingCompLocation) * force;

			OverlappingComps[x]->SetPhysicsLinearVelocity(NewVelocity, true);
		}
	}
}
