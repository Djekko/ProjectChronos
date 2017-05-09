//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#pragma once

#include "Components/ActorComponent.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_ReversableMovingObj.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROJECTCHRONOSTESTS_API UCPP_ReversableMovingObj : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCPP_ReversableMovingObj();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void CastToPlayer();

	void SetUp();

	void ProcessTransformsAndVelocities();

	void ReverseTransforms();

	void StoreTransforms();

	void ReplayTransforms();

	void ReverseVelocities();

	void StoreVelocities();

	void ReplayVelocities();

	UPROPERTY(EditAnywhere)
		bool bIsStaticObject = false;

private:

	UPROPERTY()
	AActor* Owner;

	UPROPERTY()
	UStaticMeshComponent* OwnerMesh;

	UPROPERTY(EditAnywhere)
	AProjectChronosTestsCharacter* ReversablePlayerRef;

	TArray<FTransform> TransformArray;

	TArray<FTransform> ReplayTransform;

	TArray<FTransform> VelocityArray;

	TArray<FTransform> ReplayVelocity;

	bool bResetVelocity = false;

	bool bCanReplay = false;

	int32 maxArrayLength = 400 ;
	
};
