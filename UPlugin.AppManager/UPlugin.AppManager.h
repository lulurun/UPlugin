#pragma once

#include "UPlugin/ScriptableObject.h"
#include "Poco/Thread.h"


namespace UPlugin {

class AppManagerPlugin : public UPlugin::ScriptablePluginObject<AppManagerPlugin>
{
public:
	enum STATE {
		_ERROR = -1,
		DEFAULT = 0,
		CHK_VERSION = 1,
		DOWNLOAD_PKG =2,
		EXTRACT_PKG = 3,
		INSTALL = 4,
		FINISH = 5
	};

private:
	STATE m_state;
	Poco::Thread *m_thread;

public:

	void setState(STATE s);

	AppManagerPlugin();
	AppManagerPlugin(NPP npp);
	virtual ~AppManagerPlugin();

	bool Uninstall(const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool Install(const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool GetStatus(const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool GetAppList(const NPVariant *args, uint32_t argCount, NPVariant *result);

	virtual void shutdown();

};

DECLARE_UPLUGIN_APP_CLASS(AppManagerPlugin);

}
