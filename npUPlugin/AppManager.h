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

class UPlugin_API AppManager
{
private:
	typedef ScriptablePluginObjectBase *(*PluginCreateInstance_Func)(NPP);
	typedef void(*EnableDotNetSupport_Func)(const std::string &dotnetsupport_dll);

	SharedLibrary* m_library;
	SharedLibrary* m_library_dotnetsupport;

	static std::string GetAppDLLPath(const std::string &name, const std::string &dllname);
	static std::string GetAppCreateInstanceSymbol(const std::string &name);

	void destoryScriptable();

public:
	AppManager(void);
	~AppManager(void);

	ScriptablePluginObjectBase *createScriptable(NPP npp, const std::string &name, const std::string &dllname);
};

};

#endif // __UPlugin_AppManager_H__

