// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CanClimb.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UCanClimb : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MYPROJECT_API ICanClimb
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/*virtual bool GetIsClimbableFound(EBodyState currentBodyState, FVector2D moveInputValue) = 0;
	virtual FVector GetClimbableLocation() = 0;
	virtual FRotator GetClimbableRotation(FHitResult NormalHitResult) = 0;*/
	virtual void SetupClimbingComponent() = 0;
	virtual void HandleClimbing(FVector ClimbableLocation, FRotator ClimbableFacingRotaion) = 0;
};
