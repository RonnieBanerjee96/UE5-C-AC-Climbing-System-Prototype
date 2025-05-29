// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ECharacterStates.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"


/**
 * 
 */

class AGauri;
UCLASS()
class MYPROJECT_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	public:
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "True"))
		bool canUseIK;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "True"))
		FVector ClimbableLocation;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "True"))
		bool isClimbableFound;

		UFUNCTION(BlueprintCallable)
		virtual void NativeUpdateAnimation(float Deltatime) override;
		virtual void NativeInitializeAnimation() override;

	private:
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "True"), Category = "Character")
		AGauri* GauriCharacter;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess="True"), Category = "Character")
		FVector CharacVelocity;
		UPROPERTY(VisibleAnywhere, BlueprintReadonly, meta = (AllowPrivateAccess="True"), Category = "Character")
		float CharacSpeed;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess="True"), Category = "Character")
		bool isFalling;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "True"), Category = "Character")
		float CharacRotation;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "True"), Category = "Character")
		EMovementState CharacterMovementState;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "True"), Category = "Character")
		EBodyState CharacterBodyState;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "True"), Category = "Character") EClimbingType CurrentClimbingType;
		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "True"))
		bool isWallHit;
		UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Climbable", meta = (AllowPrivateAccess = "True")) FVector ClimbableHitNormal;
		

		UFUNCTION(BlueprintCallable)
		float CalculateCharacDirection(const FVector& velocity, const FRotator& CurrentCapsuleRotation);
		


	
	
};
