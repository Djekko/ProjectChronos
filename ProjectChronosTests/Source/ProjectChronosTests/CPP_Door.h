//  Battenberg Studios. All rights reserved. No part of this publication, or any software included with it may be reproduced, stored in a retrieval system, or transmitted in any form or by any means, including photocopying, electronic, mechanical, recording or otherwise, without prior written permission of the copyright holder.

#pragma once

#include "GameFramework/Actor.h"
#include "ProjectChronosTestsCharacter.h"
#include "CPP_Door.generated.h"

UCLASS()
class PROJECTCHRONOSTESTS_API ACPP_Door : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACPP_Door();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	void CastToPlayer();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		USceneComponent* Root;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Door)
		UStaticMeshComponent* SM_Door;

	UFUNCTION(BlueprintCallable, Category = Door)
		void OpenCloseDoor();

	void lerpDoor(float DeltaSeconds);

	float LerpAlpha;

private:
	
	void SetUpMesh();

	FRotator LerpStart;
	FRotator LerpEnd;

	float lerpStartTime;

	bool bCanLerp = false;

	FQuat DoorRotation;

	bool bIsDoorOpen = false;

	float forwardsBackwards = 1.f;

	UPROPERTY(EditAnywhere)
		AActor* OwnerDoor;
		AActor* PlayerActorDoor;

	UPROPERTY(EditAnywhere)
		AProjectChronosTestsCharacter* PlayerRefDoor;
};
