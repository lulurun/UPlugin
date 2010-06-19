#include <string>
#include "DefaultPluginApp.h"
#include "UPlugin/Env.h"
#include "UPlugin/Logger.h"

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
	std::string user_dir = Env::GetInstance()->getBaseUserDIR();
	std::string prog_dir = Env::GetInstance()->getBaseProgramDIR();
	std::string config_file = Env::GetInstance()->getConfigFile();

	WARN_LOG("THIS IS DEFAULT PLUGIN");
	DEBUG_LOG("base_user_dir: " << Env::GetInstance()->getBaseUserDIR());
	DEBUG_LOG("program_dir: " << Env::GetInstance()->getBaseProgramDIR());
	DEBUG_LOG("config_file: " << Env::GetInstance()->getConfigFile());
	return true;
}

void DefaultPluginApp::shutdown() {
}

