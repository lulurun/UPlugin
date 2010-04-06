#include "UPlugin.HelloWorldDotNet.h"
#include "UPlugin/DotNetSupportHelper.h"

using namespace UPlugin::DotNet;
using namespace System;

// can be multiple
static const std::string DotNetClassName_Class1 = "HelloworldDotNet.Class1";
static const std::string DotNetDll_Class1 = "HelloworldDotNet.dll";

HelloWorldDotNetPlugin::HelloWorldDotNetPlugin()
:ScriptablePluginObject()
{
	m_identifier = "HelloWorldDotNet";
	m_use_dotnetsupport = true;
}

HelloWorldDotNetPlugin::HelloWorldDotNetPlugin(NPP npp)
	:ScriptablePluginObject(npp)
{
	m_identifier = "HelloWorldDotNet";
	m_use_dotnetsupport = true;

	register_method("say", this, &HelloWorldDotNetPlugin::say);
}

HelloWorldDotNetPlugin::~HelloWorldDotNetPlugin()
{
}

void HelloWorldDotNetPlugin::initDotNetSupport(const std::string &app_base_dir)
{
	System::String ^app_base_dir_managed = gcnew System::String(app_base_dir.c_str());
	System::String ^app_dir = System::IO::Path::Combine(app_base_dir_managed, "HelloWorldDotNet");

	// TODO @@@ support multiple objects !!!
	DotNetSupportHelper::GetInstance()->RegisterDotNetObject(
		(Managed_ScriptableBase_PTR_TYPE)this,
		gcnew System::String(DotNetClassName_Class1.c_str()),
		gcnew System::String(DotNetDll_Class1.c_str()), app_dir);
}

bool HelloWorldDotNetPlugin::say(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	try {
		Object ^obj = DotNetSupportHelper::GetInstance()->GetDotNetObject(
			(Managed_ScriptableBase_PTR_TYPE)this,
			gcnew System::String(DotNetClassName_Class1.c_str()));
		if (obj != nullptr) {
			HelloworldDotNet::Class1 ^class1 = (HelloworldDotNet::Class1 ^)obj;
			class1->say();
		}
	} catch (System::Exception ^e) {
		// TODO @@@ log
		return false;
	}
	return true;
}


