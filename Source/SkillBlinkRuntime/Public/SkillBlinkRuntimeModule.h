// Copyright (c) Marko Petric & Yevhenii Selivanov

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FSkillBlinkRuntimeModule : public IModuleInterface
{
public:
	//~IModuleInterface
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
	//~End of IModuleInterface
};
