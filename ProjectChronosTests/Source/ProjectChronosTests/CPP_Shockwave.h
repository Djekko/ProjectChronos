//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_Shockwave.generated.h"

UCLASS()
class PROJECTCHRONOSTESTS_API ACPP_Shockwave : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_Shockwave();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void SetUpMesh();

	void CastToPlayer();

	void SetShockwaveLocations();

	float fade = 0.f;
	float scale = 0.f;
	float distanceAlpha = 0.f;

	bool bFadeDown = false;

	UFUNCTION(BlueprintCallable, Category = TimeDilation)
		void PlayShockwave(float deltaTime);

	UFUNCTION(BlueprintCallable, Category = TimeDilation)
		void ReverseShockwave(float deltaTime);

	UFUNCTION(BlueprintCallable, Category = TimeDilation)
		void UpdatePerameters(float Fade, float Scale, float DistanceAlpha);

	UFUNCTION(BlueprintCallable, Category = TimeDilation)
		void OnShockwaveFinished();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Singularity)
		USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Singularity)
		UStaticMeshComponent* Shockwave;

private:

	AProjectChronosTestsCharacter* PlayerRef;

	FVector ShockWaveStartLocation;

	FVector ShockWaveEndLocation;

	AActor* OwnerShock;

	bool bSpawnedReversing;

	bool bReversingShockwave;
	
};
