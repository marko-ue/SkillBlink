// Copyright (c) Marko Petric & Yevhenii Selivanov

#include "Data/SbDataAsset.h"

// Bomber
#include "DalSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SbDataAsset)

const USbDataAsset& USbDataAsset::Get()
{
	return UDalSubsystem::GetDataAssetChecked<ThisClass>();
}