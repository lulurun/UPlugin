#ifndef __UPLUGIN_ENV_H__
#define __UPLUGIN_ENV_H__

#include <string>
#include "UPlugin/npUPlugin.h"

namespace UPlugin {

	// TODO @@@ lock !!
class UPlugin_API Env
{
private:
	Env(void);

	mutable std::string m_installdirectory;
	mutable std::string m_datadirectory;
	mutable std::string m_appbasedirectory;
	mutable std::string m_dotnetsupportdll;

public:
	virtual ~Env(void);

	const std::string &getInstallDirectory() const;
	const std::string &getDataDirectory() const;
	const std::string &getAppBaseDirectory() const;
	const std::string &getDotNetSupportDLL() const;

	static const Env *GetInstance() {
		static Env *instance = NULL;
		if (!instance) {
			instance = new Env();
		}
		return instance;
	}
};

}

#endif // __UPLUGIN_ENV_H__
