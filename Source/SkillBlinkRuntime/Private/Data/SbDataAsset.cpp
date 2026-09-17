// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "Data/SbDataAsset.h"

// Sb
#include "SbCheatExtension.h"

// Bomber
#include "DalSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbDataAsset)

const USbDataAsset& USbDataAsset::Get()
{
	return UDalSubsystem::GetDataAssetChecked<ThisClass>();
}

// Returns Blink extra tiles from the CVar if set, otherwise default value from data asset
int32 USbDataAsset::GetBlinkExtraTiles() const
{
#if !UE_BUILD_SHIPPING
	const float CVarBlinkExtraTiles = USbCheatExtension::CVarBlinkExtraTiles.GetValueOnAnyThread();
	if (CVarBlinkExtraTiles >= 0.f)
	{
		return CVarBlinkExtraTiles;
	}
#endif // !UE_BUILD_SHIPPING

	return BlinkExtraTiles;
}

// Returns whether Blink range should be infinite (pass through all tiles)
bool USbDataAsset::ShouldBlinkRangeBeInfinite() const
{
#if !UE_BUILD_SHIPPING
	const bool CVarShouldBlinkRangeBeInfinite = USbCheatExtension::CVarShouldBlinkRangeBeInfinite.GetValueOnAnyThread();
	if (CVarShouldBlinkRangeBeInfinite)
	{
		return CVarShouldBlinkRangeBeInfinite;
	}
#endif // !UE_BUILD_SHIPPING

	return bShouldBlinkRangeBeInfinite;
}