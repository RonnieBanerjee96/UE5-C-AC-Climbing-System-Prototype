// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CanClimb.h"
#include "ECharacterStates.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Gauri.generated.h"



//DECLARE_DYNAMIC_MULTICAST_DELEGATE()
class UCameraComponent;
class USkeletalMesh;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UMotionWarpingComponent;
class UCapsuleComponent;
class UClimbingComponent;


UCLASS()
class MYPROJECT_API AGauri : public ACharacter, public ICanClimb
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AGauri();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMotionWarpingComponent* MyMotionWarpingComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UCapsuleComponent* CharacterCapsuleComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State Enums")
	EBodyState CurrentBodyState;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State Enums")
	EMovementState CurrentMovementState;
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "State Enums")
	ECombatState CurrentCombatState;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State Enums")
	EWeaponState CurrentWeaponState;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Climbing Type") EClimbingType CurrentClimbingType;
	UPROPERTY()
	FString CurrentBodyStateString;
	UPROPERTY()
	FString CurrentMovementStateString;

	UPROPERTY()
	bool isClimbableFound;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector ClimbableLocation;
	UPROPERTY() FVector2D MoveInputActionValue;
	UPROPERTY() USkeletalMesh* CharacterMesh;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool isFrontWallHit;
	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bSprintState;
	UPROPERTY(BlueprintReadOnly, Category = "State")
	float SprintSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "State")
	float SpeedReductionPerFrame;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float WalkSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	float characterSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsCharacterFalling;
	UPROPERTY(BlueprintReadOnly, Category = "State")
	float characterDirection;
	UPROPERTY()
	bool bIsCharacterCrouching;
	UPROPERTY()
	float CurrentMaxSpeed;
	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsCharacterFlying;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere ,Category = "Climbable") FVector ClimbableHitNormal;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Climbable") FHitResult ClimableNormalHitResult;
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "Climbing Component") UClimbingComponent* ClimbingComponent;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCameraComponent* Camera;
	UPROPERTY(BlueprintReadWrite, EditAnywhere) float ZOffsetForCLimbing;
	UPROPERTY(EditAnywhere)
	USpringArmComponent* CameraStick;
	UPROPERTY(EditDefaultsOnly)
	UInputMappingContext* IMC;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IALook;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IAMove;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IASprint;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IAJump;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IACameraZoom;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IACrouch;
	UPROPERTY(EditDefaultsOnly)
	UInputAction* IAClimb;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* MeshComponent;
	UPROPERTY(BlueprintReadOnly, Category = "Components")
	UCharacterMovementComponent* MovementComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimationAsset* JumpAnimation;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimationAsset* TurnRAnimation;
	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimationAsset* TurnLAnimation;
	UPROPERTY(EditAnywhere, Category="Motion Warping Animation") UAnimMontage* JumpToFreeHangMontage;
	UPROPERTY(EditAnywhere, Category = "Motion Warping Animation") UAnimMontage* JumpToBracedMontage;
	UPROPERTY() UAnimMontage* ClimbingMontage;
	//UFUNCTION() void SetClimbingStateBehaviours();

	UFUNCTION() virtual void SetupClimbingComponent() override;
	UFUNCTION() void ManageBodyStateSettings(const EBodyState bodyState);

	

	
	


private:
	
	UFUNCTION() void InitializeProperties();
	UFUNCTION() void toggleSprint(const FInputActionValue& action);
	UFUNCTION() void CheckCameraOrbit();
	UFUNCTION() void Move(const FInputActionValue& action);
	UFUNCTION() void Look(const FInputActionValue& action);
	UFUNCTION() void ZoomInCamera(const FInputActionValue& action);
	UFUNCTION() void Climb(const FInputActionValue& action);
	UFUNCTION() void ReduceSpeedToWalk();
	UFUNCTION() void checkMovementStates();
	UFUNCTION() void setNecessaryValues();
	UFUNCTION() void checkBodyStates();
	UFUNCTION()void StartJump(const FInputActionValue& action);
	UFUNCTION() void startCrouch(const FInputActionValue& action);
	//UFUNCTION() void HandleClimbing();
	UFUNCTION() virtual void HandleClimbing(FVector ClimbableHitLocation, FRotator ClimbableFacingRotaion) override;


	UFUNCTION()
	void checkFrontWallHit();


	UFUNCTION()
	void PrintStates();

	UPROPERTY()
	FTimerHandle WalkRunTimer;

	UPROPERTY()
	FTimerHandle CameraZoomTimer;

	UPROPERTY()
	FHitResult wallHitResult;
	
	UPROPERTY()
	TArray<AActor*> OwnerCharacter;

	UPROPERTY()
	FHitResult ClimbableHitResult;
	UPROPERTY()
	TArray<AActor*> ClimbableActors;
	UPROPERTY()
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesArray;
	UPROPERTY()
	FHitResult ClimbableTopHitResult;
	UPROPERTY()
	FHitResult ClimbableBraceHitResult;

	UPROPERTY() bool isZoomingIn;

	UFUNCTION() void ManageCameraBasedOnStates(const EBodyState BodyState);
	
	UPROPERTY() FVector LeftHandBoneLocation;
	UPROPERTY() FVector RightHandBoneLocation;
	
	
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
