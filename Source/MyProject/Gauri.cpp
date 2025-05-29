// Fill out your copyright notice in the Description page of Project Settings.


#include "Gauri.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ClimbingComponent.h"
#include "MotionWarpingComponent.h"


// Sets default values
AGauri::AGauri()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	if (!CharacterMovementComponentName.IsNone()) MovementComponent = Cast<UCharacterMovementComponent>(GetDefaultSubobjectByName(CharacterMovementComponentName));
	
	if (!MeshComponentName.IsNone()) MeshComponent = Cast<USkeletalMeshComponent>(GetDefaultSubobjectByName(MeshComponentName));

	if (GetCapsuleComponent()) CharacterCapsuleComponent = Cast<UCapsuleComponent>(GetDefaultSubobjectByName(CapsuleComponentName));

	CharacterMesh = MeshComponent->GetSkeletalMeshAsset();
	ZOffsetForCLimbing = 137;
	PrimaryActorTick.bCanEverTick = true;
	SprintSpeed = 600.f;
	WalkSpeed = 200.f;
	SpeedReductionPerFrame = 50.f;
	bSprintState = true;
	CameraStick = CreateDefaultSubobject<USpringArmComponent>(TEXT("Camera Stick"));
	CameraStick->SetupAttachment(RootComponent);
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraStick);
	OwnerCharacter.Add(GetOwner());
	MyMotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MW component"));
	ClimbingComponent = CreateDefaultSubobject<UClimbingComponent>(TEXT("Climbing Component"));
	
	
	
	
	



}



void AGauri::InitializeProperties()
{
	
	CameraStick->bUsePawnControlRotation = true;
	CameraStick->bInheritPitch = true;
	CameraStick->bInheritYaw = true;
	CameraStick->bInheritRoll = false;
	CameraStick->SocketOffset = FVector(0, 25, 20);
	CameraStick->TargetArmLength = 180;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bIsCharacterFalling = false;
	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECC_GameTraceChannel1));
	bUseControllerRotationYaw = false;
	GetRootComponent()->bHiddenInGame = false;
}

// Called when the game starts or when spawned
void AGauri::BeginPlay()
{
	Super::BeginPlay();
	InitializeProperties();
	SetupClimbingComponent();
	
	
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		if (UEnhancedInputLocalPlayerSubsystem* EISubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			EISubSystem->ClearAllMappings();
			EISubSystem->AddMappingContext(IMC, 0);
		}
	}
	
	
}

void AGauri::CheckCameraOrbit()
{
	if (CurrentMovementState == EMovementState::StateIdling || CurrentBodyState == EBodyState::StateJumping || CurrentBodyState == EBodyState::StateClimbing)
	{
		bUseControllerRotationYaw = false;
	}
	else
	{
		bUseControllerRotationYaw = true;
	}
}

void AGauri::Move(const FInputActionValue& action)
{
	
	MoveInputActionValue = action.Get<FVector2D>();
	FVector Forward = GetActorForwardVector();
	FVector Side = GetActorRightVector();

	if (CurrentBodyState != EBodyState::StateClimbing) {
		if (MoveInputActionValue.Y != 0) {
			if (MoveInputActionValue.Y < 0 && isFrontWallHit) {
			AddMovementInput(Forward, MoveInputActionValue.Y);
			}
			else if (!isFrontWallHit) {
				AddMovementInput(Forward, MoveInputActionValue.Y);
			}
		}
		if(MoveInputActionValue.X != 0){
			AddMovementInput(Side, MoveInputActionValue.X);
		}
	}
	else
	{
		bool canMoveFurtherLeft = ClimbingComponent->canMoveFurtherLeft(MeshComponent->GetBoneLocation("LeftHand"));
		bool canMoveFurtherRight = ClimbingComponent->canMoveFurtherRight(MeshComponent->GetBoneLocation("RightHand"));
		if ((!canMoveFurtherLeft && MoveInputActionValue.X < 0) || (!canMoveFurtherRight && MoveInputActionValue.X > 0)) AddMovementInput(Side, 0);
		else AddMovementInput(Side, MoveInputActionValue.X);
		
		
	}
}

void AGauri::Look(const FInputActionValue& action)
{
	FVector2D actionValue = action.Get<FVector2D>();
	if (GetController())
	{
		AddControllerYawInput(actionValue.X);
		AddControllerPitchInput(actionValue.Y * -1);

	}

}


void AGauri::ZoomInCamera(const FInputActionValue& action)
{
	if (action.Get<bool>() && CurrentBodyState != EBodyState::StateClimbing) {
		GEngine->AddOnScreenDebugMessage(3, 3, FColor::Cyan, TEXT("Triggered Zoom"));
		isZoomingIn = true;
		
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(3,3, FColor::Cyan, TEXT("Zoomed OUt"));
		isZoomingIn = false;
		/*CameraStick->SocketOffset = FVector(0, 25, 20);
		CameraStick->TargetArmLength = 180;*/
	}
}

void AGauri::Climb(const FInputActionValue& action)
{
	if (action.Get<bool>())
	{
		ClimbingComponent->Climb();
	}
}





void AGauri::StartJump(const FInputActionValue& action)
{
	if (CurrentBodyState == EBodyState::StateStanding) {
	AddMovementInput(GetActorForwardVector(), 0);
	AddMovementInput(GetActorRightVector(), 0);
	Jump();
	}

	if(CurrentBodyState == EBodyState::StateCrouching)
	{
		UnCrouch();
	}
	

}

void AGauri::startCrouch(const FInputActionValue& action)
{
	if (CurrentBodyState == EBodyState::StateStanding) {
	Crouch();
	GEngine->AddOnScreenDebugMessage(6, 1, FColor::Red, TEXT("Crouch Called"));
	}
	else if (CurrentBodyState == EBodyState::StateCrouching) {
		UnCrouch();
		GEngine->AddOnScreenDebugMessage(6, 1, FColor::Red, TEXT("Uncrouch Called"));
	}
	else if (CurrentBodyState == EBodyState::StateClimbing) {
		MovementComponent->SetMovementMode(EMovementMode::MOVE_Falling);
	}
}




void AGauri::HandleClimbing(FVector ClimbableHitLocation, FRotator ClimbableFacingRotaion)
{
	
		//isClimbableFound = checkForClimbables(MoveInputActionValue);
		if (ClimbingComponent)
		{
			

			//Set Movement Mode To FLying for gravity controls
			MovementComponent->SetMovementMode(EMovementMode::MOVE_Flying);
			//Disable Collision
			CharacterCapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Ignore);
			//Allow Motion Warping to Handle Rotation
			FVector AdjustedLocation = FVector(ClimbableHitLocation.X, ClimbableHitLocation.Y, ClimbableHitLocation.Z-135);
			MovementComponent->bUseControllerDesiredRotation = false;
			bUseControllerRotationYaw = false;
			ClimbingMontage = JumpToFreeHangMontage;
			
			MyMotionWarpingComponent->AddOrUpdateWarpTargetFromLocationAndRotation(FName("climb"), AdjustedLocation, ClimbableFacingRotaion);
			PlayAnimMontage(ClimbingMontage, 1.5f);
			GEngine->AddOnScreenDebugMessage(4, 100.0f, FColor::Red, FString::Printf(TEXT("Actor Rotation: %s"), *GetActorRotation().ToString()));
		}
		
		CharacterCapsuleComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Block);
}

void AGauri::checkFrontWallHit()
{
	isFrontWallHit = UKismetSystemLibrary::SphereTraceSingle(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorForwardVector() * 24, 9,
		UEngineTypes::ConvertToTraceType(ECC_Visibility), false, OwnerCharacter, EDrawDebugTrace::None, wallHitResult,
		true, FLinearColor::Red, FLinearColor::Green, 2.0F);
	/*if (isFrontWallHit)
	{
		MovementComponent->MaxWalkSpeed = 100;
	}*/
}


void AGauri::PrintStates()
{
	switch (CurrentBodyState)
	{
	case EBodyState::StateStanding:
		CurrentBodyStateString = "StateStanding";
		break;
	case EBodyState::StateCrouching:
		CurrentBodyStateString = "StateCrouching";
		break;
	case EBodyState::StateJumping:
		CurrentBodyStateString = "StateJumping";
		break;
	case EBodyState::StateClimbing:
		CurrentBodyStateString = "State Climbing";
		break;
	default:
		CurrentBodyStateString = "Unknown";
		break;
	}
	switch (CurrentMovementState)
	{
	case EMovementState::StateSprinting:
		CurrentMovementStateString = "StateSprinting";
		break;
	case EMovementState::StateWalking:
		CurrentMovementStateString = "StateWalking";
		break;
	case EMovementState::StateIdling:
		CurrentMovementStateString = "StateIdling";
		break;

	default:
		CurrentMovementStateString = "Unknown";
		break;
	}
}



void AGauri::SetupClimbingComponent()
{
	UE_LOG(LogTemp, Warning, TEXT("SetupClimbingComponent called"));
	ClimbingComponent = FindComponentByClass<UClimbingComponent>();
	if (!ClimbingComponent) {
		ClimbingComponent = CreateDefaultSubobject<UClimbingComponent>("Climbing Component");
		UE_LOG(LogTemp, Warning, TEXT("SetupClimbingComponent insde if"));
	}
}



void AGauri::ManageBodyStateSettings(const EBodyState currentBodyState)
{
	if (CurrentBodyState == EBodyState::StateClimbing)
	{
		CharacterCapsuleComponent->SetCapsuleSize(23,
			40);
	}

	//CHANGE AND REFACTOR TO ACCOUNT FOR INDIVIDUAL POSSIBLE STATES WHEN NECESSARY:

	else if (CurrentBodyState == EBodyState::StateStanding)
	{
		
		CharacterCapsuleComponent->SetCapsuleSize(23,
			75);
		checkFrontWallHit();

	}
	
}

void AGauri::ManageCameraBasedOnStates(const EBodyState currentBodyState)
{
	if (currentBodyState == EBodyState::StateClimbing)
	{
		CameraStick->TargetArmLength = 350;
		CameraStick->SocketOffset = FVector(-100, 40, -10);
	}
	else if (currentBodyState != EBodyState::StateClimbing && !isZoomingIn)
	{
		CameraStick->SocketOffset = FVector(0, 25, 20);
		CameraStick->TargetArmLength = 180;
	}
	else if (currentBodyState != EBodyState::StateClimbing && isZoomingIn)
	{
		CameraStick->TargetArmLength = 100;
		CameraStick->SocketOffset = FVector(0, 45, 30);
	}
}

void AGauri::ReduceSpeedToWalk()
{
	if (MovementComponent->MaxWalkSpeed > WalkSpeed) {
		MovementComponent->MaxWalkSpeed = FMath::Max(MovementComponent->MaxWalkSpeed - SpeedReductionPerFrame * 0.05f, WalkSpeed);
		GEngine->AddOnScreenDebugMessage(1, 3, FColor::Red, FString::Printf(TEXT("Current Max Speed: %f"), CurrentMaxSpeed));
	}

}

void AGauri::setNecessaryValues()
{
	CurrentMaxSpeed = MovementComponent->GetMaxSpeed();
	characterSpeed = GetVelocity().Length();
	bIsCharacterFalling = MovementComponent->IsFalling();
	bIsCharacterCrouching = MovementComponent->IsCrouching();
	bIsCharacterFlying = MovementComponent->IsFlying();
	GEngine->AddOnScreenDebugMessage(1, 6, FColor::Red, FString::Printf(TEXT("Current Speed: %f"), characterSpeed));
	

}

void AGauri::checkBodyStates()
{
	if (bIsCharacterCrouching)
		CurrentBodyState = EBodyState::StateCrouching;

	if (!bIsCharacterFalling && !bIsCharacterCrouching)
		CurrentBodyState = EBodyState::StateStanding;

	if (bIsCharacterFalling)
		CurrentBodyState = EBodyState::StateJumping;
	if(bIsCharacterFlying)
		CurrentBodyState = EBodyState::StateClimbing;
	
}
void AGauri::checkMovementStates()
{
	if (characterSpeed == 0 && !isFrontWallHit) {
		CurrentMovementState = EMovementState::StateIdling;
	}
	else if(characterSpeed <= WalkSpeed)
	{
		CurrentMovementState = EMovementState::StateWalking;
	}
	else if (characterSpeed > WalkSpeed)
	{
		CurrentMovementState = EMovementState::StateSprinting;
	}
}



void AGauri::toggleSprint(const FInputActionValue& action) {
	
	if (CurrentMovementState == EMovementState::StateSprinting && CurrentBodyState != EBodyState::StateJumping)
		GetWorldTimerManager().SetTimer(WalkRunTimer, this, &AGauri::ReduceSpeedToWalk, 0.01f, true);
	else if (CurrentMovementState != EMovementState::StateSprinting && CurrentBodyState == EBodyState::StateStanding)
	{
		GetWorldTimerManager().ClearTimer(WalkRunTimer);
		MovementComponent->MaxWalkSpeed = 600.f;
	}
		
		GEngine->AddOnScreenDebugMessage(0, 2.f, FColor::Blue, FString::Printf(TEXT("MovementSpeed: %f"), MovementComponent->MaxWalkSpeed));

}




// Called every frame
void AGauri::Tick(float DeltaTime)
{
	
	Super::Tick(DeltaTime);
	setNecessaryValues();
	checkMovementStates();
	checkBodyStates();
	CheckCameraOrbit();
	ManageBodyStateSettings(CurrentBodyState);
	ManageCameraBasedOnStates(CurrentBodyState);
	PrintStates();
	GEngine->AddOnScreenDebugMessage(10, 1.0f, FColor::Red, FString::Printf(TEXT("Body State: %s, Movement State: %s"), *CurrentBodyStateString, *CurrentMovementStateString));
	GEngine->AddOnScreenDebugMessage(8, 1.0f, FColor::Yellow, FString::Printf(TEXT("Actor Location: %s"), *GetActorLocation().ToString()));
	
		
}

// Called to bind functionality to input
void AGauri::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EIComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	EIComponent->BindAction(IAMove, ETriggerEvent::Triggered, this, &AGauri::Move);
	EIComponent->BindAction(IALook, ETriggerEvent::Triggered, this, &AGauri::Look);
	EIComponent->BindAction(IASprint, ETriggerEvent::Triggered, this, &AGauri::toggleSprint);
	EIComponent->BindAction(IAJump, ETriggerEvent::Triggered, this, &AGauri::StartJump);
	EIComponent->BindAction(IACameraZoom, ETriggerEvent::Triggered, this, &AGauri::ZoomInCamera);
	EIComponent->BindAction(IACameraZoom, ETriggerEvent::Completed, this, &AGauri::ZoomInCamera);
	EIComponent->BindAction(IACrouch, ETriggerEvent::Triggered, this, &AGauri::startCrouch);
	EIComponent->BindAction(IAClimb, ETriggerEvent::Triggered, this, &AGauri::Climb);
	//EIComponent->BindAction(IAJump, ETriggerEvent::Completed, this, &AGauri::StopJump);

}

