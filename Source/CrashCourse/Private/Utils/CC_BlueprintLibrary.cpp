// Copyright eeeee


#include "Utils/CC_BlueprintLibrary.h"

EHitDirection UCC_BlueprintLibrary::GetHitDirection(const FVector& TargetForward, const FVector& ToInstigator)
{
	const float Dot = FVector::DotProduct( TargetForward, ToInstigator);
	if (Dot < -0.5f)
	{
		return EHitDirection::Back;
	}
	else if (Dot < .5f)
	{
		// Either Left or Right
		const FVector Cross = FVector::CrossProduct(TargetForward, ToInstigator);
		if (Cross.Z < .0f)
		{
			return EHitDirection::Left;
		}
		return EHitDirection::Right;
	}
	return EHitDirection::Forward;
}

FName UCC_BlueprintLibrary::GetHitDirectionName(const EHitDirection& HitDirection) 
{
	switch (HitDirection)
	{
		case EHitDirection::Forward: return FName("Forward");
		case EHitDirection::Back: return FName("Back");
		case EHitDirection::Right: return FName("Right");
		case EHitDirection::Left: return FName("Left");
		default: return FName("None");
	}
}
