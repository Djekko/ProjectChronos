//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_TimeDihalationArea.generated.h"

UCLASS()
class PROJECTCHRONOSTESTS_API ACPP_TimeDihalationArea : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_TimeDihalationArea();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void SetUpMesh();

	void CastToPlayer();

	void SetUp();

	float scale = 0.f;
	float totalTime = 0.f;

	UFUNCTION(BlueprintCallable, Category = TimeDilation)
	void Expand(float deltaSeconds);

	UFUNCTION(BlueprintCallable, Category = TimeDilation)
	void ReverseExpand(float deltaSeconds);

	UFUNCTION(BlueprintCallable, Category = TimeDilation)
	void SetVolumeScale(float Scale);

	UFUNCTION(BlueprintCallable, Category = TimeDilation)
	void StoreValuesForRewindingTime();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Singularity)
		USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Singularity)
		UStaticMeshComponent* TimeVolume;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void FakeSlowDilation(UPrimitiveComponent* TargetComp, bool bEnablegravity);

	void FakeNormalDilation(UPrimitiveComponent* TargetComp);

	void TickFunction();

protected:

	UPROPERTY()
		AActor* OwnerTDA;

	UPROPERTY()
		UStaticMeshComponent* OwnerMeshTDA;

	UPROPERTY()
		AProjectChronosTestsCharacter* PlayerRef;

	bool bSpawnedReversing;

	bool bReversingFreezeVolume;

	float timeDilation = 0.05f;
	
};
