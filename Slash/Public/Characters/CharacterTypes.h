#pragma once

UENUM(BlueprintType)
enum class ETigrealState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_OneHandEquipped UMETA(DisplayName = "Equipped One Handed Weapon"),
	ECS_TwoHandEquipped UMETA(DisplayName = "Equipped Two Handed Weapon")
};

enum class EActionState : uint8
{
	EAS_UnOccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Equipping UMETA(DisplayName = "Equipping")
};

enum class ELandState : uint8
{
	ELS_Locked UMETA(DisplayName = "Locked"), 
	ELC_Unlocked UMETA(DisplayName = "Unlocked")
};