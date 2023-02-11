// Copyright Epic Games, Inc. All Rights Reserved.
// f4x 2023
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FDriftIslandPluginModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
