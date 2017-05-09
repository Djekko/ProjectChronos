// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "ProjectChronosTests.h"
#include "ProjectChronosTestsGameMode.h"
#include "ProjectChronosTestsHUD.h"
#include "ProjectChronosTestsCharacter.h"

AProjectChronosTestsGameMode::AProjectChronosTestsGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AProjectChronosTestsHUD::StaticClass();
}
