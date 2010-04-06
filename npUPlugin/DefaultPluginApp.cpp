#include <windows.h>
#include "DefaultPluginApp.h"

using namespace UPlugin;

DefaultPluginApp::DefaultPluginApp(NPP npp)
	:ScriptablePluginObjectBase(npp)
{
	m_identifier = "DefaultPluginApp";
}

DefaultPluginApp::~DefaultPluginApp(void)
{
}

bool DefaultPluginApp::HasMethod(NPIdentifier name)
{
	return true;
}

bool DefaultPluginApp::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
#ifdef UPLUGIN_DOTNET
	// TOTO @@@ remove managed code !!
	MessageBox(NULL, "THIS IS DEFAULT PLUGIN", "UPLUGIN", MB_OK);
#endif
	return true;
}

