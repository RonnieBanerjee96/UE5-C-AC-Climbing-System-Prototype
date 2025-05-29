// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CanClimb.h"
#include "ECharacterStates.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClimbingComponent.generated.h"

class AGauri;
class USkeletalMeshComponent;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UClimbingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UClimbingComponent();
	ICanClimb* IClimbingCharacter;

	UPROPERTY() AActor* ComponentOwner;
	UPROPERTY() AGauri* PlayerCharacter;
	UPROPERTY() FHitResult ClimbableHitResult;
	UPROPERTY() FHitResult ClimbableNormalHitResult;
	UPROPERTY() TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	UPROPERTY() TArray<AActor*> OwnerCharacter;
	UPROPERTY() EBodyState OwnerCurrentBodyState;
	UPROPERTY() FVector2D MoveInputValue;

	UPROPERTY() FHitResult HitResultLeftSideFromHit;
	UPROPERTY() FHitResult HitResultRightSideFromHit;



	UFUNCTION() bool CheckForClimbable(FVector2D moveInputActionValue);
	UFUNCTION() bool FindClimbableNormal(FVector climbableHitImpactPoint);
	UFUNCTION() FRotator AdjustRotationToClimbable(FHitResult NormalHitResult);
	UFUNCTION() FVector AdjustLocationForClimbing(FVector ClimableImpactPoint);
	UFUNCTION() bool canMoveFurtherRight(FVector RightHandBoneLocation);
	UFUNCTION() bool canMoveFurtherLeft(FVector LeftHandBoneLocation);
	UFUNCTION() void Climb();
	UFUNCTION() void Vault();
	UPROPERTY() FVector ClimbableHitImpactPoint;
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
