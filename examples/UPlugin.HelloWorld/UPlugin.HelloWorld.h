#pragma once

#include "UPlugin/ScriptableObject.h"

using namespace System;
using namespace UPlugin;

class UPlugin_API HelloWorldPlugin : public ScriptablePluginObject<HelloWorldPlugin>
{
private:
	std::string _message;
public:
	HelloWorldPlugin()
		: ScriptablePluginObject()
	{
	}
	HelloWorldPlugin(NPP npp)
		: ScriptablePluginObject(npp)
	{
		_message = "Hello World";
		register_method("say", this, &HelloWorldPlugin::say);
	};
	virtual ~HelloWorldPlugin() {
	};

	bool say(const NPVariant *args, uint32_t argCount, NPVariant *result) {
		System::Windows::Forms::MessageBox::Show(gcnew System::String(_message.c_str()));
		return true;
	};

};

DECLARE_UPLUGIN_APP_CLASS(HelloWorldPlugin);
