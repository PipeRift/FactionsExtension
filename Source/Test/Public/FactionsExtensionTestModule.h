// Copyright 2015-2026 Piperift. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"


class FFactionsExtensionTestModule : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}

	static inline FFactionsExtensionTestModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FFactionsExtensionTestModule>("FactionsExtensionTest");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("FactionsExtensionTest");
	}
};
