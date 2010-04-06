
#include "UPlugin/ScriptableObject.h"
#include "UPlugin/Env.h"
#include "UPlugin/DotNetSupportHelper.h"
#include "AppManager.h"
#include "DefaultPluginApp.h"

//#include "Poco/ClassLoader.h"
#include "Poco/Manifest.h"
#include "Poco/Exception.h"

using namespace Poco;
using namespace UPlugin;

static const std::string CREATEINSTANCE_FUNC_PREFIX = "CreateInstance_";
static const std::string CREATEINSTANCE_FUNC_SUFFIX = "Plugin";
static const std::string APP_DLL_PREFIX = "UPlugin.";
static const std::string APP_DLL_SUFFIX = ".dll";

AppManager::AppManager(void)
	:m_library(NULL)
{
}

AppManager::~AppManager(void)
{
	this->destoryScriptable();
}

ScriptablePluginObjectBase *AppManager::createScriptable(NPP npp, const std::string &name, const std::string &dllname)
{
	std::string path = GetAppDLLPath(name, dllname);
	std::string createInstanceSymbol = GetAppCreateInstanceSymbol(name);

	ScriptablePluginObjectBase* obj = NULL;
	try {

		m_library = new SharedLibrary(path);

		if (m_library->hasSymbol(createInstanceSymbol))
		{
			PluginCreateInstance_Func createInstanceFunc = (PluginCreateInstance_Func) m_library->getSymbol(createInstanceSymbol);
			obj = (*createInstanceFunc)(npp);
			if (obj->useDotNetSupport()) {
#ifdef UPLUGIN_DOTNET
				obj->initDotNetSupport(Env::GetInstance()->getAppBaseDirectory());
#else
				// TODO @@@ load managed code !!
				throw 1;
#endif
			}
		} else {
			// TODO @@@ log not a correct plugin app
			throw 1;
		}
	} catch (std::exception &e) {
		// TODO @@ log
		return (ScriptablePluginObjectBase *)NPN_CreateObject(npp, GET_NPOBJECT_CLASS(DefaultPluginApp));
	} catch (...) {
		// TODO @@ log
		return (ScriptablePluginObjectBase *)NPN_CreateObject(npp, GET_NPOBJECT_CLASS(DefaultPluginApp));
	}
	if (obj == NULL) {
		return (ScriptablePluginObjectBase *)NPN_CreateObject(npp, GET_NPOBJECT_CLASS(DefaultPluginApp));
	} else {
		return obj;
	}
}

void AppManager::destoryScriptable() {
	if (m_library) {
		m_library->unload();
		delete m_library;
	}
	if (m_library_dotnetsupport) {
		m_library_dotnetsupport->unload();
		delete m_library_dotnetsupport;
	}
}

std::string AppManager::GetAppDLLPath(const std::string &name, const std::string &dllname) {
	std::string app_dll_name;
	if (dllname.empty()) {
		app_dll_name = APP_DLL_PREFIX + name + APP_DLL_SUFFIX;
	} else {
		app_dll_name = dllname;
	}
	return Env::GetInstance()->getAppBaseDirectory() + "\\" + name + "\\" + app_dll_name;
}

std::string AppManager::GetAppCreateInstanceSymbol(const std::string &name) {
	return CREATEINSTANCE_FUNC_PREFIX + name + CREATEINSTANCE_FUNC_SUFFIX;
}


