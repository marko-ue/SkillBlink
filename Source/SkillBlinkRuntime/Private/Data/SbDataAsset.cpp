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
int32 USbDataAsset::GetBlinkTileRange() const
{
#if !UE_BUILD_SHIPPING
	const float CVarBlinkTileRange = USbCheatExtension::CVarBlinkTileRange.GetValueOnAnyThread();
	if (CVarBlinkTileRange >= 0.f)
	{
		return CVarBlinkTileRange;
	}
#endif // !UE_BUILD_SHIPPING

	return BlinkTileRange;
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