//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_Ladder.generated.h"

UCLASS()
class PROJECTCHRONOSTESTS_API ACPP_Ladder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_Ladder();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void CastToPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderTop)
		USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderTop)
		UStaticMeshComponent* SM_Ladder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderTop)
		UStaticMeshComponent* connectionArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderTop)
		UStaticMeshComponent* directionArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderTop)
		UBoxComponent* CollisionBox;

private:

	void SetUpMesh();

	void LineTrace();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void LerpPlayer(float DeltaSeconds);
	float alpha;

	FVector LerpStartPos;
	FVector LerpEndPos;

	FQuat LerpStartQuat;
	FQuat LerpEndQuat;

	float lerpStartTime;

	bool bCanLerp = false;

	UPROPERTY(EditAnywhere)
	AActor* OwnerLadder;
	AActor* PlayerActorLadder;

	UPROPERTY(EditAnywhere)
	AProjectChronosTestsCharacter* PlayerRefLadder;
	
};
