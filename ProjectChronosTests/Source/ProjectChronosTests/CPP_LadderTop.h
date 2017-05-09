//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_LadderTop.generated.h"

UCLASS()
class PROJECTCHRONOSTESTS_API ACPP_LadderTop : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_LadderTop();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void CastToPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderTop)
		USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderTop)
		UStaticMeshComponent* SM_LadderTop;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderTop)
		UStaticMeshComponent* SM_LadderSide;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderTop)
		UStaticMeshComponent* connectionArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderTop)
		UStaticMeshComponent* directionArrow;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderTop)
		UBoxComponent* MainBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderTop)
		UBoxComponent* BackupBox;

private:

	void SetUpMesh();

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void LerpPlayer(float DeltaSeconds);
	float alpha;

	FVector lerpStart;
	FVector lerpEnd;

	float lerpStartTime;

	bool bCanLerp = false;

	UPROPERTY(EditAnywhere)
		AActor* OwnerLadderTop;
		AActor* PlayerActorLadderTop;

	UPROPERTY(EditAnywhere)
		AProjectChronosTestsCharacter* PlayerRefLadderTop;
	
};
