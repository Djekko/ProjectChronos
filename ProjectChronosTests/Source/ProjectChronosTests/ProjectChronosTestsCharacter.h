// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/Character.h"
#include "ProjectChronosTestsCharacter.generated.h"

class UInputComponent;

UCLASS(config=Game)
class AProjectChronosTestsCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;
public:
	AProjectChronosTestsCharacter();

	virtual void BeginPlay();

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer)
		int32 Minutes = 5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer)
		int32 Seconds = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Timer)
		float Timer = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
		float testCasting = 213.f;

	UFUNCTION( Category = test)
		void TestCode();

	///Creates a line trace
	UFUNCTION(BlueprintCallable, Category = LineTrace)
		void LineTrace();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = LineTrace)
		void DisplayQueHud();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = LineTrace)
		void RemoveQueHud();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LineTrace)
		bool bLineTraceHitSomething;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LineTrace)
		bool bInteractionQueHudVisable = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LineTrace)
		float reach = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LineTrace)
		AActor* LineTraceActorHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = LineTrace)
		UPrimitiveComponent* LineTraceComponentHit;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Ageable)
		bool bAgeableInteract = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Interaction)
		float interactionStartTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Interaction)
		bool bInterationPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Ability)
		int32 AbilityNumber = 0;

	UFUNCTION(Category = Energy)
		void ChangeAbilityUp();

	UFUNCTION(Category = Energy)
		void ChangeAbilityDown();

	UFUNCTION(Category = Energy)
		void ActivateAbility1();

	UFUNCTION(Category = Energy)
		void ActivateAbility2();

	UFUNCTION(Category = Energy)
		void ActivateAbility3();

	UFUNCTION(Category = Energy)
		void ActivateAbility4();

	UFUNCTION(BlueprintCallable, Category = Energy)
		bool ReduceEnergy(float reductionAmount);

	UFUNCTION(BlueprintCallable, Category = Energy)
		void DecayEnergy(float DeltaSeconds);

	UFUNCTION(BlueprintCallable, Category = Energy)
		void RegenerateEnergy(float DeltaSeconds);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Energy)
		bool bEnergyDecaying = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Energy)
		bool bEnergyRegenerating = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Energy)
		float PlayerEnergy = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Energy)
		float PlayerMaxEnergy = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Energy)
		bool bIsInDamageArea = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Energy)
		float damageStartTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Energy)
		bool bFirstTimeDamage = true;

	UFUNCTION(BlueprintCallable, Category = Energy)
		void AddPeriodicDamage();

	UFUNCTION(BlueprintImplementableEvent, Category = Energy)
		void ShakeCam(FVector origin);

	UFUNCTION(BlueprintImplementableEvent, Category = Energy)
		void GameOver();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Energy)
		FVector DamageLocation;

	UPROPERTY()
		UCameraShake* DamageShake;

	UFUNCTION(BlueprintCallable, Category = TimeVolume)
		void SpawnTimeVolumeGhost();

	UFUNCTION(BlueprintCallable, Category = TimeVolume)
		void SpawnTimeVolume();

	UFUNCTION(Category = TimeVolume)
		void RemoveTimeVolumeGhost();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TimeVolume)
		bool bTimeVolumeGhostSpawned;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TimeVolume)
		bool bPlayerInTimeVolume;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TimeVolume)
		AActor* TimeVolumeGhostRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TimeVolume)
		AActor* TimeVolumeRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TimeVolume)
		float dihalationAreaDistance = 750.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TimeVolume)
		TSubclassOf<class ACPP_TimeDihalationArea> TimeDihalationAreaBP;

	UFUNCTION(BlueprintCallable, Category = Singularity)
		void SpawnSingularity();

	float SingularityDistFromPlayer = 500.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Singularity)
		TSubclassOf<class ACPP_Singularity> SingularityBP;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = AI)
		TSubclassOf<class AActor> SwatRagdollBP;

	UFUNCTION(BlueprintCallable, Category = Shockwave)
		void SpawnShockwave();

	UFUNCTION(BlueprintCallable, Category = Shockwave)
		void ShockwaveEffect();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Shockwave)
		void RagdollAI(AActor* HitAI);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Shockwave)
		AActor* ShockwaveRef;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Shockwave)
		float shockwaveForce = 4.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Shockwave)
		TSubclassOf<class ACPP_Shockwave> ShockwaveBP;

	UFUNCTION(Category = SpawningBlueprints)
		void FindBlueprints();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		bool bReverseButtonPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<TSubclassOf<AActor>>ActorsDestroyed;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<float>DestroyedTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<FTransform>DestroyedTransform;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<AActor*>ActorsSpawned;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<float>SpawnedTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<FTransform>SpawnedTransform;

	UFUNCTION(BlueprintImplementableEvent, Category = Rewind)
		void AddRewindFilter();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		float playRate = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		float rewindStartTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		float rewindEndTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		int32 numOfTimeEnergy;

	UFUNCTION(BlueprintCallable, Category = Rewind)
		void ProcessPlayerState();

	UFUNCTION(Category = Rewind)
		void ProcessPlayerTransform();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<FTransform> PlayerTransArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<FTransform> ReplayPlayerTransArray;

	UFUNCTION(Category = Rewind)
		void ProcessPlayerVelocity();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<FTransform> PlayerVelocArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<FTransform> ReplayPlayerVelocArray;

	UFUNCTION(Category = Rewind)
		void ProcessPlayerRotation();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<FRotator> PlayerRotArray;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<FRotator> ReplayPlayerRotArray;

	UFUNCTION(BlueprintCallable, Category = Rewind)
		void RespawnAbilitiesWhileRewinding();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		int32 respawnAbilityNum = 0;

	UFUNCTION(BlueprintCallable, Category = Rewind)
		void ReplayAbilitiesAfterRewinding();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		int32 replayAbilityNum = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		bool bAbilityReplayFlag;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Rewind)
		TArray<float> SpawnArrayTemp;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = Rewind)
		void StopTimeRewinding();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderMovement)
		bool bCanClimbLadder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LadderMovement)
		bool bIsFacingLadder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Moevement)
		bool bCanSprint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Moevement)
		bool bIsSprinting;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Moevement)
		bool bIsCrouching;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Moevement)
		bool bCanUnCrouch;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Moevement)
		float crouchSpeed = 300.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Moevement)
		float walkSpeed = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Moevement)
		float sprintSpeed = 1000.f;

	UFUNCTION(BlueprintCallable, Category = Door)
		void DoorDispatch();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	bool bInvertedY = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	bool bInvertedX = false;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AProjectChronosTestsProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

protected:
	
	/** Fires a projectile. */
	void OnFire();

	void OnSprint();

	UFUNCTION(BlueprintCallable, Category = Movement)
	void OnSprintStop();

	void PlayerCrouch();

	void PlayerUnCrouch();

	void PlayerStand();

	UFUNCTION(BlueprintCallable, Category = Movement)
	void TickTryUnCrouch();

	UFUNCTION(BlueprintCallable, Category = Movement)
	void UpdateCrouchAnimation(FVector newScale);

	UFUNCTION(BlueprintImplementableEvent, Category = Movement)
	void PlayCrouchAnimation();

	UFUNCTION(BlueprintImplementableEvent, Category = Movement)
	void PlayStandAnimation();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	struct TouchData
	{
		TouchData() { bIsPressed = false;Location=FVector::ZeroVector;}
		bool bIsPressed;
		ETouchIndex::Type FingerIndex;
		FVector Location;
		bool bMoved;
	};
	void BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location);
	void TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location);
	TouchData	TouchItem;
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

	/* 
	 * Configures input for touchscreen devices if there is a valid touch interface for doing so 
	 *
	 * @param	InputComponent	The input component pointer to bind controls to
	 * @returns true if touch controls were enabled.
	 */
	bool EnableTouchscreenMovement(UInputComponent* InputComponent);

public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

};

