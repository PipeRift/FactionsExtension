// Copyright 2015-2019 Piperift. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"


class FFactionsExtensionTest : public IModuleInterface
{
public:

	virtual void StartupModule() override {}
	virtual void ShutdownModule() override {}

	static inline FFactionsExtensionTest& Get() {
		return FModuleManager::LoadModuleChecked<FFactionsExtensionTest>("FactionsExtensionTest");
	}

	static inline bool IsAvailable() {
		return FModuleManager::Get().IsModuleLoaded("FactionsExtensionTest");
	}
};

IMPLEMENT_MODULE(FFactionsExtensionTest, FactionsExtensionTest);
