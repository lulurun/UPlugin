#pragma once

#include "UPlugin/ScriptableObject.h"
#include "UPlugin/Logger.h"

//using namespace System;
using namespace UPlugin;

class HelloWorldPlugin : public ScriptablePluginObject<HelloWorldPlugin>
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
		m_identifier = "HelloWorld";

		_message = "Hello World";
		register_method("say", this, &HelloWorldPlugin::say);
	};
	virtual ~HelloWorldPlugin() {
	};

	bool say(const NPVariant *args, uint32_t argCount, NPVariant *result) {
		//System::Windows::Forms::MessageBox::Show(gcnew System::String(_message.c_str()));
		ERROR_LOG(this->_message);
		return true;
	};

	// called from UPlugin when UPlugin is unloaded
	virtual void shutdown() { /* nothing to be done */ };
};

DECLARE_UPLUGIN_APP_CLASS(HelloWorldPlugin);
