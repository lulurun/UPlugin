#ifndef __UPLUGIN_ENV_H__
#define __UPLUGIN_ENV_H__

#include <string>
#include "UPlugin/npUPlugin.h"
#include "Poco/Util/IniFileConfiguration.h"
#include "Poco/AutoPtr.h"

namespace UPlugin {

	static const std::string APP_DLL_PREFIX = "UPlugin.";
	static const std::string APP_DLL_SUFFIX = ".dll";

	// TODO @@@ lock !!
class UPlugin_API Env
{
private:
	Env(void);

	std::string m_installdirectory;
	std::string m_datadirectory;
	std::string m_appbasedirectory;
	std::string m_dotnetsupportdll;

	std::string m_base_prog_dir;
	std::string m_base_user_dir;
	std::string m_base_user_low_dir; // for > vista
	std::string m_ini_conf_file;

	Poco::AutoPtr<Poco::Util::IniFileConfiguration> m_config;

public:
	virtual ~Env(void);

	const std::string &getInstallDirectory() const;
	const std::string &getDataDirectory() const;
	const std::string &getAppBaseDirectory() const;
	const std::string &getDotNetSupportDLL() const;

	const std::string &getBaseUserDIR() const;
	const std::string &getBaseProgramDIR() const;
	const std::string &getConfigFile() const;
	const Poco::Util::IniFileConfiguration *getConfig() const;

	static const Env *GetInstance() {
		static Env *instance = NULL;
		if (!instance) {
			instance = new Env();
		}
		return instance;
	}

public:
	static std::string GetUPluginAPPDLL(const std::string &app_name) {
		return APP_DLL_PREFIX + app_name + APP_DLL_SUFFIX;
	}
};

}

#endif // __UPLUGIN_ENV_H__
