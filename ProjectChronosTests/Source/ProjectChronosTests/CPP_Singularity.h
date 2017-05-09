//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_Singularity.generated.h"

UCLASS()
class PROJECTCHRONOSTESTS_API ACPP_Singularity : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this actor's properties
	ACPP_Singularity();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void SetUpMesh();

	void CastToPlayer();

	float scale = 0.f;
	float totalTime = 0.f;

	UFUNCTION(BlueprintCallable, Category = Singularity)
		void ExpandSingularity(float deltaSeconds);

	UFUNCTION(BlueprintCallable, Category = Singularity)
		void ReverseSingularity(float deltaSeconds);

	UFUNCTION(BlueprintCallable, Category = Singularity)
		void UpdateExpanding(float Scale);

	UFUNCTION(BlueprintCallable, Category = Singularity)
		void FinishedExpanding();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void TickFunction();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Singularity)
		USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Singularity)
		UStaticMeshComponent* BlackHole;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Singularity)
		UStaticMeshComponent* AreaOfEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Singularity)
		UParticleSystemComponent* SingularityParticles;

private:

	UPROPERTY(EditAnywhere)
		bool bSpawnedRevering;
		bool bReversingSingularity;

	UPROPERTY(EditAnywhere)
		float force = 0.1f;

	UPROPERTY(EditAnywhere)
		AActor* SingularityOwner;
		AActor* SingularityPlayerActor;

	UPROPERTY(EditAnywhere)
		AProjectChronosTestsCharacter* SingularityPlayerRef;

public:

	UStaticMeshComponent* MeshComp;

	UMaterial* ShaderMaterial;

	UMaterialInstanceDynamic* ShaderMatInst;
};
