// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Gauri.h"
#include "Kismet/KismetMathLibrary.h"

void UMyAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);
	if (GauriCharacter) {
		
		isFalling = GauriCharacter->GetMovementComponent()->IsFalling();
		isWallHit = GauriCharacter->isFrontWallHit;
		CharacVelocity = GauriCharacter->GetMovementComponent()->Velocity;
		CharacSpeed = GauriCharacter->GetMovementComponent()->Velocity.Size();
		CharacRotation = CalculateCharacDirection(CharacVelocity, GauriCharacter->GetActorRotation());
		CharacterBodyState = GauriCharacter->CurrentBodyState;
		CharacterMovementState = GauriCharacter->CurrentMovementState;
		GEngine->AddOnScreenDebugMessage(2, 2, FColor::Black, FString(isFalling? "Falling": "Not Falling"));
		GEngine->AddOnScreenDebugMessage(2, 2, FColor::Black, FString::Printf(TEXT("Chra Speed: %f"), CharacSpeed));
		isClimbableFound = GauriCharacter->isClimbableFound;
		if (isClimbableFound)
		{
			ClimbableHitNormal = GauriCharacter->ClimbableHitNormal;
			ClimbableLocation = GauriCharacter->ClimbableLocation;
			FString climableString = *ClimbableLocation.ToString();
			CurrentClimbingType = GauriCharacter->CurrentClimbingType;
			GEngine->AddOnScreenDebugMessage(3, 2, FColor::Black, FString::Printf(TEXT("Climbable Location: %s"), *climableString));
		}
		
		
	}
}

void UMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	canUseIK = false;
	GEngine->AddOnScreenDebugMessage(2, 2, FColor::Black, FString::Printf(TEXT("Calling from AnimInstance %f"), CharacSpeed));
	GauriCharacter = Cast<AGauri>(TryGetPawnOwner());
}

float UMyAnimInstance::CalculateCharacDirection(const FVector& Velocity, const FRotator& CurrentCapsuleRotation)
{
	if (Velocity.SizeSquared2D() == 0) {
		return 0.0f;
	}
	FRotator currentRotation = Velocity.Rotation();
	float delRotation = UKismetMathLibrary::NormalizedDeltaRotator(currentRotation, CurrentCapsuleRotation).Yaw;
	return delRotation;
}
