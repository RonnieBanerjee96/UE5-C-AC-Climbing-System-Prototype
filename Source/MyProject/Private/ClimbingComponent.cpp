// Fill out your copyright notice in the Description page of Project Settings.


#include "ClimbingComponent.h"
#include "..\Gauri.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UClimbingComponent::UClimbingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	
	PrimaryComponentTick.bCanEverTick = true;
	OwnerCharacter.Add(ComponentOwner);
	OwnerCurrentBodyState = EBodyState::StateStanding;
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	MoveInputValue = FVector2D(0, 0);
	
	
	// ...
}


bool UClimbingComponent::CheckForClimbable(FVector2D moveInputValue)
{
		
	bool isHit = false;
	float XInputValue = moveInputValue.X;
	float YInputValue = moveInputValue.Y;
	float ZTraceDirection = 155;
	
	


	//CHECK FOR CLIMBABLES WHEN ON GROUND:
	if (OwnerCurrentBodyState != EBodyState::StateClimbing) {
		for (int i = 1; i < 15; i++) {
			isHit = UKismetSystemLibrary::CapsuleTraceSingleForObjects(GetWorld(), ComponentOwner->GetActorLocation(), ComponentOwner->GetActorLocation()
				+ ComponentOwner->GetActorForwardVector() * (i + 1) * 12, 12, 90 , ObjectTypesArray, false, OwnerCharacter,
				EDrawDebugTrace::ForDuration,ClimbableHitResult, true, FLinearColor::Green, FLinearColor::Blue, 1.0f);

			if (isHit) {
				GEngine->AddOnScreenDebugMessage(2, 10, FColor::Red, FString::Printf(TEXT("Hit Actor: %s"),*ClimbableHitResult.GetActor()->GetName()));
				ClimbableHitImpactPoint = ClimbableHitResult.ImpactPoint;
				break;
				
			}
		}
	}
	//CHECK FOR CLIMBABLES WHEN CLIMBING
	else
	{
		if (YInputValue != 0)
		{
			ZTraceDirection = (125 + (20 * YInputValue)) * YInputValue;					  //THIS CALCULATION IS MEANT TO ADJUST THE CAPSULE TRACE DIRECTION IN Z AXIS,TAKING  BASE OFFSET OF 125 FROM THE-
																						  //-PLAYER IN Z AXIS, IT MAKES THE CAPSULE HIGHER BY 20 UNITS IN UPWARD TRACE, AND LOWER BY 10 UNITS IN DOWNWARD TRACE.
			                                                                              //-THIS SHOULD WORK.

			int TraceIter = (YInputValue * YInputValue) + (3 * (XInputValue * XInputValue)) + 1;  //THIS WILL DETERMINE HOW MANY TIMES THE FOR LOOP SHOULD RUN, THERE'S NO POINT RUNNING THE LOOP-
																								  //-UNECESSARILY WHEN SIDE INPUTS ARE NOT PROVIDED.

			for (int i = 1; i <= TraceIter; i++)
			{
				isHit = UKismetSystemLibrary::CapsuleTraceSingleForObjects(GetWorld(), FVector(ComponentOwner->GetActorLocation().X - 2, ComponentOwner->GetActorLocation().Y - 2, 
					ComponentOwner->GetActorLocation().Z + ZTraceDirection), FVector(ComponentOwner->GetActorLocation().X, ComponentOwner->GetActorLocation().Y, 
						ComponentOwner->GetActorLocation().Z + ZTraceDirection) + ComponentOwner->GetActorRightVector() * 12 * XInputValue * i, 12, 60, 
					ObjectTypesArray, false, OwnerCharacter, EDrawDebugTrace::ForDuration, ClimbableHitResult, true, FLinearColor::Green, FLinearColor::Blue, 1.0f);
				if (isHit)
				{
					if (GEngine && &ClimbableHitResult) GEngine->AddOnScreenDebugMessage(2, 10, FColor::Red, FString::Printf(TEXT("Hit Actor: %s"),
						*ClimbableHitResult.GetActor()->GetName()));
					ClimbableHitImpactPoint = ClimbableHitResult.ImpactPoint;
					break;
					
				}
			}

		}

		
	}

	return isHit;
	
}


bool UClimbingComponent::FindClimbableNormal(FVector climbableHitImpactPoint)
{
	bool isClimbableNormalFound = UKismetSystemLibrary::LineTraceSingle(GetWorld(),
		FVector(ComponentOwner->GetActorLocation().X, ComponentOwner->GetActorLocation().Y + 50, climbableHitImpactPoint.Z + 1),
		FVector(climbableHitImpactPoint.X - 2, climbableHitImpactPoint.Y - 2, climbableHitImpactPoint.Z + 1),
		UEngineTypes::ConvertToTraceType(ECC_EngineTraceChannel1), false,
		OwnerCharacter, EDrawDebugTrace::ForDuration, ClimbableNormalHitResult, true);

	return isClimbableNormalFound;
}

FRotator UClimbingComponent::AdjustRotationToClimbable(FHitResult NormalHitResult)
{
	FRotator ClimbableRotation = NormalHitResult.ImpactNormal.Rotation().GetInverse();
	ClimbableRotation.Pitch = 0.0f;
	ClimbableRotation.Roll = 0.0f;
	if (FMath::Abs(ClimbableRotation.Yaw) < 1)
	{
		ClimbableRotation.Yaw = 0.0f;
	}
	return ClimbableRotation;
}

FVector UClimbingComponent::AdjustLocationForClimbing(FVector ClimbableImpactPoint)
{
	float LeftSideHitXDistanceFromCenter =0;
	float RightSideHitXDistanceFromCenter =0;
	FVector XOffsetAdjustedLocation = ClimbableImpactPoint;
	FHitResult isLeftSideHitHR;

	bool isLeftSideHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
		FVector(ClimbableImpactPoint.X - 30, ClimbableImpactPoint.Y + 20, ClimbableImpactPoint.Z + 1),
		FVector(ClimbableImpactPoint.X - 30, ClimbableImpactPoint.Y - 2, ClimbableImpactPoint.Z + 1),
		ObjectTypesArray, false, OwnerCharacter, EDrawDebugTrace::ForDuration,
		isLeftSideHitHR, true);

	bool isRightSideHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
		FVector(ClimbableImpactPoint.X + 30, ClimbableImpactPoint.Y + 20, ClimbableImpactPoint.Z + 1),
		FVector(ClimbableImpactPoint.X + 30, ClimbableImpactPoint.Y - 2, ClimbableImpactPoint.Z + 1),
		ObjectTypesArray, false, OwnerCharacter, EDrawDebugTrace::ForDuration,
		isLeftSideHitHR, true);

	if (!isLeftSideHit)
	{

		for (int i = 0; i < 10; i++)
		{
			bool isTraceXDirectionTowardsClimbableHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), FVector(ClimbableImpactPoint.X-50 + (5 * (i)), ClimbableImpactPoint.Y+20, ClimbableImpactPoint.Z+1),
				FVector(ClimbableImpactPoint.X - 50 + (5 * (i)), ClimbableImpactPoint.Y-2, ClimbableImpactPoint.Z+1),
				ObjectTypesArray, false, OwnerCharacter, EDrawDebugTrace::ForDuration, HitResultLeftSideFromHit, true,
				FColor::Cyan, FColor::Red, 10.0f);
			if (isTraceXDirectionTowardsClimbableHit)
			{
				LeftSideHitXDistanceFromCenter = (ClimbableImpactPoint.X - 50) - (HitResultLeftSideFromHit.ImpactPoint.X);
				if (LeftSideHitXDistanceFromCenter <= 5) LeftSideHitXDistanceFromCenter = 25;
				XOffsetAdjustedLocation = FVector(ClimbableImpactPoint.X + LeftSideHitXDistanceFromCenter, ClimbableImpactPoint.Y, ClimbableImpactPoint.Z);
				break;
			}
		}
	}

	
	else if (!isRightSideHit)
	{
		for (int i=0; i  < 10; i++)
		{

			bool isTraceXDirectionTowardsClimbableHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), FVector(ClimbableImpactPoint.X + 40 - (5 * (i)), ClimbableImpactPoint.Y + 20, ClimbableImpactPoint.Z + 1),
				FVector(ClimbableImpactPoint.X + 40 - (5 * (i)), ClimbableImpactPoint.Y - 2, ClimbableImpactPoint.Z + 1),
				ObjectTypesArray, false, OwnerCharacter, EDrawDebugTrace::ForDuration, HitResultRightSideFromHit, true,
				FColor::Red, FColor::Cyan, 10.0f);
			if (isTraceXDirectionTowardsClimbableHit)
			{
				RightSideHitXDistanceFromCenter = (ClimbableImpactPoint.X + 50) - (HitResultRightSideFromHit.ImpactPoint.X);
				if (RightSideHitXDistanceFromCenter <=5) RightSideHitXDistanceFromCenter = 25;
				XOffsetAdjustedLocation = FVector(ClimbableImpactPoint.X - RightSideHitXDistanceFromCenter, ClimbableImpactPoint.Y, ClimbableImpactPoint.Z);
				break;
			}
		}
	}
	

	return XOffsetAdjustedLocation;
}

bool UClimbingComponent::canMoveFurtherRight(FVector RightHandBoneLocation)
{
	FHitResult RightSideHR;
	bool isHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
		FVector(RightHandBoneLocation.X + 10, ClimbableHitImpactPoint.Y + 10, ClimbableHitImpactPoint.Z + 1),
		FVector(RightHandBoneLocation.X + 10, ClimbableHitImpactPoint.Y - 2, ClimbableHitImpactPoint.Z + 1),
		ObjectTypesArray, false, OwnerCharacter, EDrawDebugTrace::ForDuration, RightSideHR, true);
	
	return isHit;
	
}

bool UClimbingComponent::canMoveFurtherLeft(FVector LeftHandBoneLocation)
{
	FHitResult LefSideHR;
	bool isHit = UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(),
		FVector(LeftHandBoneLocation.X -10, ClimbableHitImpactPoint.Y + 10, ClimbableHitImpactPoint.Z + 1),
		FVector(LeftHandBoneLocation.X - 10, ClimbableHitImpactPoint.Y - 2, ClimbableHitImpactPoint.Z + 1),
		ObjectTypesArray, false, OwnerCharacter, EDrawDebugTrace::ForDuration, LefSideHR, true);

	return isHit;
}

void UClimbingComponent::Climb()
{
	if (IClimbingCharacter) {
		bool isClimbableFound = CheckForClimbable(MoveInputValue);
		if (isClimbableFound)
		{
			bool isClimbableNormalFound = FindClimbableNormal(ClimbableHitImpactPoint);
			if (isClimbableNormalFound)
			{
			FRotator AdjustedRotation = AdjustRotationToClimbable(ClimbableNormalHitResult);
			IClimbingCharacter = Cast<ICanClimb>(GetOwner());
			FVector AdjustedLocation = AdjustLocationForClimbing(ClimbableHitImpactPoint);
			if (AdjustedRotation.Yaw != 0) IClimbingCharacter->HandleClimbing(AdjustedLocation, AdjustedRotation);
			}
		}
	}
}



void UClimbingComponent::Vault()
{
}

// Called when the game starts
void UClimbingComponent::BeginPlay()
{
	Super::BeginPlay();
	ComponentOwner = GetOwner();
	PlayerCharacter = Cast<AGauri>(ComponentOwner);
	IClimbingCharacter = Cast<ICanClimb>(ComponentOwner);
	// ...
	
}


// Called every frame
void UClimbingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (PlayerCharacter) {
		MoveInputValue = PlayerCharacter->MoveInputActionValue;
		OwnerCurrentBodyState = PlayerCharacter->CurrentBodyState;
		
	}
	// ...
}

