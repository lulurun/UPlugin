#ifndef __UPlugin_AppManager_H__
#define __UPlugin_AppManager_H__

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
#include <map>

using namespace Poco;

namespace UPlugin {

class AppManager
{
private:
	typedef ScriptablePluginObjectBase *(*PluginCreateInstance_Func)(NPP);
	AppManager(void);
	~AppManager(void);

	SharedLibrary* m_library;

	static std::string GetAppDLLPath(const std::string &name, const std::string &dllname);
	static std::string GetAppCreateInstanceSymbol(const std::string &name);

public:
	ScriptablePluginObjectBase *createScriptable(NPP npp, const std::string &name, const std::string &dllname);
	void destoryScriptable();

	static AppManager *Instance() {
		static AppManager *_instance = NULL;
		if (_instance == NULL) {
			_instance = new AppManager();
		}
		return _instance;
	};

};

};

#endif // __UPlugin_AppManager_H__

