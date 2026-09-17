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
	const float CvarBlinkExtraTiles = USbCheatExtension::CVarBlinkExtraTiles.GetValueOnAnyThread();
	if (CvarBlinkExtraTiles >= 0.f)
	{
		return CvarBlinkExtraTiles;
	}
#endif // !UE_BUILD_SHIPPING

	return BlinkExtraTiles;
}