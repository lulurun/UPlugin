
#include "UPlugin/ScriptableObject.h"
#include "UPlugin/Env.h"
#include "UPlugin/Logger.h"
#include "UPlugin/DotNetSupportHelper.h"
#include "AppManager.h"
#include "DefaultPluginApp.h"

//#include "Poco/ClassLoader.h"
#include "Poco/Path.h"
#include "Poco/Manifest.h"
#include "Poco/Exception.h"
#include <map>

using namespace Poco;
using namespace UPlugin;

static const std::string CREATEINSTANCE_FUNC_PREFIX = "CreateInstance_";
static const std::string CREATEINSTANCE_FUNC_SUFFIX = "Plugin";

static const std::string NAME_APP_MANAGER = "AppManager";
static const std::string NAME_APP_MANAGER_DLL_FILE = "UPlugin.AppManager.dll";

AppManager::AppManager(void)
	:m_library(NULL)
{
	TRACE_LOG("AppManager");
}

AppManager::~AppManager(void)
{
	TRACE_LOG("~AppManager enter");
	//this->destoryScriptable();
	TRACE_LOG("~AppManager leave");
}

ScriptablePluginObjectBase *AppManager::createScriptable(NPP npp, const std::string &name, const std::string &dllname)
{
	std::string path = GetAppDLLPath(name, dllname);
	std::string createInstanceSymbol = GetAppCreateInstanceSymbol(name);

	DEBUG_LOG("CreateScriptable: " << path << " " << createInstanceSymbol);

	ScriptablePluginObjectBase* obj = NULL;
	try {

		Poco::SharedLibrary *lib = new SharedLibrary(path);
		if (lib->hasSymbol(createInstanceSymbol))
		{
			PluginCreateInstance_Func createInstanceFunc = (PluginCreateInstance_Func) lib->getSymbol(createInstanceSymbol);
			obj = (*createInstanceFunc)(npp);
			if (obj->useDotNetSupport()) {
#ifdef UPLUGIN_DOTNET
				if (0 == name.compare(NAME_APP_MANAGER)) {
					// @@@ special process for AppManager
					obj->initDotNetSupport(Env::GetInstance()->getInstallDirectory());
				}
				else {
					obj->initDotNetSupport(Env::GetInstance()->getAppBaseDirectory());
				}
#else // UPLUGIN_DOTNET
				// TODO @@@ load managed code !!
				throw 1;
#endif // UPLUGIN_DOTNET
			}
			// @@@ register to map
			Poco::Thread::TID tid = Poco::Thread::currentTid();
			ThreadLibraryList_TYPE::iterator it = this->_lib_map.find(tid);
			if (it != this->_lib_map.end()) {
				it->second->push_back(lib);
			} else {
				// the first one
				LibraryList_TYPE *list = new LibraryList_TYPE;
				list->push_back(lib);
				this->_lib_map[tid] = list;
			}
		} else {
			ERROR_LOG("can not find " << createInstanceSymbol << " in " << path);
			throw 1;
		}
	} catch (std::exception &e) {
		ERROR_LOG("Exception create instance of " << name << ": " << e.what());
		return (ScriptablePluginObjectBase *)NPN_CreateObject(npp, GET_NPOBJECT_CLASS(DefaultPluginApp));
	} catch (...) {
		ERROR_LOG("Error create instance of " << name);
		return (ScriptablePluginObjectBase *)NPN_CreateObject(npp, GET_NPOBJECT_CLASS(DefaultPluginApp));
	}
	if (obj == NULL) {
		ERROR_LOG("failed create instance of " << name << ": obj is NULL");
		return (ScriptablePluginObjectBase *)NPN_CreateObject(npp, GET_NPOBJECT_CLASS(DefaultPluginApp));
	} else {
		return obj;
	}
}

void AppManager::clearLoadedLibrary(const std::string &name) {
	TRACE_LOG("clearLoadedLibrary enter");
	std::string path = GetAppDLLPath(name, ""); // TODO @@@ dllname !!
	Poco::Path p(path);
	if (!p.isFile()) {
		ERROR_LOG("clearLoadedLibrary: can not find dll for " << name);
		return;
	}
	Poco::Thread::TID tid = Poco::Thread::currentTid();
	ThreadLibraryList_TYPE::iterator it = this->_lib_map.find(tid);
	if (it == this->_lib_map.end()) {
		INFO_LOG("clearLoadedLibrary: found nothing " << tid);
	} else {
		LibraryList_TYPE *list = it->second;
		for(LibraryList_TYPE::iterator lit=list->begin(); lit!=list->end(); lit++) {
			if ((*lit)) {
				if ((*lit)->getPath() == path) {
					INFO_LOG("clearLoadedLibraries: unload " << (*lit)->getPath() << " " << tid);
					(*lit)->unload();
					delete (*lit);
				}
			}
		}
		list->clear();
		delete list;
		this->_lib_map.erase(it);
	}
	TRACE_LOG("clearLoadedLibrary leave");

}

void AppManager::clearAllLoadedLibraries() {
	TRACE_LOG("clearAllLoadedLibraries enter");
	Poco::Thread::TID tid = Poco::Thread::currentTid();
	ThreadLibraryList_TYPE::iterator it = this->_lib_map.find(tid);
	if (it == this->_lib_map.end()) {
		INFO_LOG("clearAllLoadedLibraries: found nothing " << tid);
	} else {
		LibraryList_TYPE *list = it->second;
		for(LibraryList_TYPE::iterator lit=list->begin(); lit!=list->end(); lit++) {
			if ((*lit)) {
				INFO_LOG("clearAllLoadedLibraries: unload " << (*lit)->getPath() << " " << tid);
				(*lit)->unload();
				delete (*lit);
			}
		}
		list->clear();
		delete list;
		this->_lib_map.erase(it);
	}
	TRACE_LOG("clearLoadedLibraries leave");
}

void AppManager::destoryScriptable() {
/*
	if (m_library) {
		m_library->unload();
		delete m_library;
	}
*/
	/*
	if (m_library_dotnetsupport) {
		m_library_dotnetsupport->unload();
		delete m_library_dotnetsupport;
	}
	*/
}

std::string AppManager::GetAppDLLPath(const std::string &name, const std::string &dllname) {
	if (name == NAME_APP_MANAGER) {
		// @@@ special for AppManager
		Poco::Path p(Env::GetInstance()->getInstallDirectory());
		p.append(NAME_APP_MANAGER_DLL_FILE);
		return p.toString();
	}
	else {
		std::string app_dll_name;
		if (dllname.empty()) {
			app_dll_name = Env::GetUPluginAPPDLL(name);
		} else {
			app_dll_name = dllname;
		}
		Poco::Path p(Env::GetInstance()->getAppBaseDirectory());
		p.append(name).append(app_dll_name);
		return p.toString();
	}
}

std::string AppManager::GetAppCreateInstanceSymbol(const std::string &name) {
	return CREATEINSTANCE_FUNC_PREFIX + name + CREATEINSTANCE_FUNC_SUFFIX;
}


