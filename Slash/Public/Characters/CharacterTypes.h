#pragma once

UENUM(BlueprintType)
enum class ETigrealState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_OneHandEquipped UMETA(DisplayName = "Equipped One Handed Weapon"),
	ECS_TwoHandEquipped UMETA(DisplayName = "Equipped Two Handed Weapon")
};
UENUM(BlueprintType)
enum class EActionState : uint8
{
	EAS_UnOccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "HitReaction"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Equipping UMETA(DisplayName = "Equipping")
};
UENUM(BlueprintType)
enum class ELandState : uint8
{
	ELS_Locked UMETA(DisplayName = "Locked"), 
	ELC_Unlocked UMETA(DisplayName = "Unlocked")
};
UENUM(BlueprintType)
enum class EDeathPose : uint8
{
	EDP_Alive UMETA(DisplayName = "Alive"),
	EDP_FromFront UMETA(DisplayName = "FromFront"),
	EDP_FromBack UMETA(DisplayName = "FromBack"),
	EDP_FromLeft UMETA(DisplayName = "FromLeft"),
	EDP_FromRight UMETA(DisplayName = "FromRight")
};
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged"),
	EES_NoState UMETA(DisplayName = "NoState")

};