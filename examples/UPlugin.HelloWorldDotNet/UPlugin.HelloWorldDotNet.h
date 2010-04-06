#pragma once

#include "UPlugin/ScriptableObject.h"

using namespace System;
using namespace UPlugin;

class HelloWorldDotNetPlugin : public ScriptablePluginObject<HelloWorldDotNetPlugin>
{
private:

public:
	HelloWorldDotNetPlugin();
	HelloWorldDotNetPlugin(NPP npp);
	virtual ~HelloWorldDotNetPlugin();

	bool say(const NPVariant *args, uint32_t argCount, NPVariant *result);

	virtual void initDotNetSupport(const std::string &app_base_dir);
};

DECLARE_UPLUGIN_APP_CLASS(HelloWorldDotNetPlugin);
