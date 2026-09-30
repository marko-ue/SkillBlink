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

// Returns whether corner blinks should chain through consecutive corners
bool USbDataAsset::ShouldBlinkChainThroughCorners() const
{
#if !UE_BUILD_SHIPPING
	const bool CVarShouldBlinkChainThroughCorners = USbCheatExtension::CVarShouldBlinkChainThroughCorners.GetValueOnAnyThread();
	if (CVarShouldBlinkChainThroughCorners)
	{
		return CVarShouldBlinkChainThroughCorners;
	}
#endif // !UE_BUILD_SHIPPING

	return CVarShouldBlinkChainThroughCorners;
}
