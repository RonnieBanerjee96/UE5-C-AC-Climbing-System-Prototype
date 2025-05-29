// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "CoreMinimal.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class EMovementState : uint8
{
	StateSprinting UMETA(DisplayName="Sprinting State"),
	StateWalking UMETA(DisplayName= "Walking State"),
	StateIdling UMETA(DisplayName = "Idling"),
	
};

UENUM(BlueprintType)
enum class EBodyState : uint8
{
	StateStanding UMETA(DisplayName = "Standing State"),
	StateCrouching UMETA(DisplayName = "Crouching State"),
	StateJumping UMETA(DisplayName = "Jumping State"),
	StateClimbing UMETA(DisplayName = "Climbing State"),
	/*StateOnGround UMETA(DisplayName = "On Ground State"),*/
};
UENUM(BlueprintType)
enum class EWeaponState : uint8 {
	StateUnarmed UMETA(DisplayName = "Unarmed State"),
	StateOneHanded UMETA(DisplayName = "Equipped: One Handed Weapon"),
	StateTwoHanded UMETA(DisplayName = "Equipped: Two Handed Weapon"),
	StateWeaponAndShield UMETA(DisplayName = "Equipped: One Handed Weapon and Shield")
};
UENUM(BlueprintType)
enum class ECombatState : uint8 {
	StateInCombat UMETA(DisplayName= "In Combat State"),
	StateOutOfCombat UMETA(DsiplayName = "Out OF Combat State")
};

UENUM(BlueprintType)
enum class EClimbingType : uint8 {
	StateClimbBraced UMETA(DisplayName = "Braced"),
	StateClimbHang UMETA(DisplayName = "Hanging")
};