//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#pragma once

#include "Components/ActorComponent.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_Rottable.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROJECTCHRONOSTESTS_API UCPP_Rottable : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCPP_Rottable();

	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

	void CastToPlayer();

	void SetUpMesh();

	void CheckForRotting(float DeltaSeconds);

	void RotObject(float DeltaSeconds);

	void DissolveObject(float DeltaSeconds);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rottable)
		bool bObjectHit;
		bool bHasRotted;
		bool bIsRotting;
		bool bHasDissolved;
		bool bIsDissolving;
		bool bHasStartedToAge;
		bool bIsDestroyed = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Rottable)
		float startTime;
		float perameterValue;

	UPROPERTY(EditAnywhere)
		UMaterial* DynamicMat;

private:
	UPROPERTY(EditAnywhere)
		UMaterialInstanceDynamic* MatInstance; //Material Instance for Object

	UPROPERTY(EditAnywhere)
		AActor* Owner;
		AActor* PlayerActor;

	UPROPERTY(EditAnywhere)
		UStaticMeshComponent* OwnerMesh;

	UPROPERTY(EditAnywhere)
		AProjectChronosTestsCharacter* PlayerRef;
};
