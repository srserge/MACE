// Copyright srSerge. All Rights Reserved.

#include "Combat/MaceCombatLibrary.h"

#include "GameFramework/Actor.h"

namespace
{
	/** Half the width of the front and back quadrants, in degrees. */
	constexpr float QuadrantHalfWidth = 45.0f;
}

EMaceHitDirection UMaceCombatLibrary::GetHitDirection(const AActor* Target, FVector HitOrigin, float& OutAngle)
{
	OutAngle = 0.0f;

	if (!Target)
	{
		return EMaceHitDirection::Front;
	}

	// Flattened to the ground plane. Height says nothing about which side was
	// hit, and left in it would tilt every answer when the two characters stand
	// at different heights.
	const FVector ToOrigin = (HitOrigin - Target->GetActorLocation()).GetSafeNormal2D();
	if (ToOrigin.IsNearlyZero())
	{
		return EMaceHitDirection::Front;
	}

	const FVector Forward = Target->GetActorForwardVector().GetSafeNormal2D();

	// Signed angle from facing to origin. Unreal is left-handed with Z up, so a
	// positive Z on the cross product means the origin is on the right.
	const float Cos = FVector::DotProduct(Forward, ToOrigin);
	const float Sin = FVector::CrossProduct(Forward, ToOrigin).Z;
	OutAngle = FMath::RadiansToDegrees(FMath::Atan2(Sin, Cos));

	const float AbsAngle = FMath::Abs(OutAngle);

	if (AbsAngle <= QuadrantHalfWidth)
	{
		return EMaceHitDirection::Front;
	}

	if (AbsAngle >= 180.0f - QuadrantHalfWidth)
	{
		return EMaceHitDirection::Back;
	}

	return (OutAngle > 0.0f) ? EMaceHitDirection::Right : EMaceHitDirection::Left;
}
