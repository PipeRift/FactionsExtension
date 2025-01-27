// Copyright 2015-2023 Piperift. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"


class FFactionsTest : public IModuleInterface
{
public:
	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}

	static inline FFactionsTest& Get()
	{
		return FModuleManager::LoadModuleChecked<FFactionsTest>("FactionsTest");
	}

	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("FactionsTest");
	}
};

IMPLEMENT_MODULE(FFactionsTest, FactionsTest);
