#ifndef __UPlugin_AppManager_H__
#define __UPlugin_AppManager_H__

#include <map>
#include <list>
#include <string>
#include "UPlugin/ScriptableBase.h"

#include "Poco/Foundation.h"
#include "Poco/MetaObject.h"
#include "Poco/Manifest.h"
#include "Poco/SharedLibrary.h"
#include "Poco/Mutex.h"
#include "Poco/Exception.h"
#include "Poco/UnicodeConverter.h"
#include "Poco/UnWindows.h"
#include "Poco/Thread.h"

using namespace Poco;

namespace UPlugin {

class AppManager
{
private:
	typedef ScriptablePluginObjectBase *(*PluginCreateInstance_Func)(NPP);
	typedef void(*EnableDotNetSupport_Func)(const std::string &dotnetsupport_dll);

	SharedLibrary* m_library;
	SharedLibrary* m_library_dotnetsupport;

	static std::string GetAppDLLPath(const std::string &name, const std::string &dllname);
	static std::string GetAppCreateInstanceSymbol(const std::string &name);

	void destoryScriptable();

	AppManager(void);
public:
	~AppManager(void);
	void clearLoadedLibrary(const std::string &name);
	void clearAllLoadedLibraries();

	ScriptablePluginObjectBase *createScriptable(NPP npp, const std::string &name, const std::string &dllname);

	typedef std::list<Poco::SharedLibrary *> LibraryList_TYPE;
	typedef std::map<Poco::Thread::TID, LibraryList_TYPE*> ThreadLibraryList_TYPE;
	ThreadLibraryList_TYPE _lib_map;

	static AppManager *GetInstance() {
		static AppManager* instance = NULL;
		if (instance == NULL) {
			instance = new AppManager();
		}
		return instance;
	}
};

};

#endif // __UPlugin_AppManager_H__

