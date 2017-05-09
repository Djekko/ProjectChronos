//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_TimeDihalationAreaGhost.generated.h"

UCLASS()
class PROJECTCHRONOSTESTS_API ACPP_TimeDihalationAreaGhost : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_TimeDihalationAreaGhost();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void SetUpMesh();

	void CastToPlayer();

	void SetUp();

	void UpdateGhostInfrontOfPlayer();

	FVector NewGhostPos();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Singularity)
		USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Singularity)
		UStaticMeshComponent* TimeVolume;

private:

	UPROPERTY()
		AActor* OwnerTDAG;

	UPROPERTY()
		UStaticMeshComponent* OwnerMeshTDAG;

	UPROPERTY()
		AProjectChronosTestsCharacter* PlayerRef;
	
};
