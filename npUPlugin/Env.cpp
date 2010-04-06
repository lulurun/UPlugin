#include <shlobj.h>
#include <sstream>
#include "UPlugin/Env.h"

using namespace std;
using namespace UPlugin;

static const std::string _uplugin_identifier = "UPlugin";
static const std::string _base_directory = "E:\\github\\UPlugin\\bin";

Env::Env(void)
{
}

Env::~Env(void)
{
}

const string &Env::getInstallDirectory() const
{
	if (m_installdirectory.empty()) {
#ifdef _DEBUG
		m_installdirectory.append(_base_directory);
#else
		m_installdirectory.append(".");
		char buf[MAX_PATH];
		SHGetSpecialFolderPath(NULL, buf, CSIDL_PROGRAM_FILES, FALSE);
		m_installdirectory = buf;
		m_installdirectory.append("\\");
		m_installdirectory.append(_uplugin_identifier);
#endif
	}
	return m_installdirectory;
}

const string &Env::getDataDirectory() const
{
	if (m_datadirectory.empty()) {
#ifdef _DEBUG
		m_datadirectory.append(_base_directory);
#else
		m_datadirectory.append(".");
		OSVERSIONINFO osVersionInfo;
		osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		GetVersionEx(&osVersionInfo);

		if (osVersionInfo.dwMajorVersion == 5) {
			// XP
			char buf[MAX_PATH];
			SHGetSpecialFolderPath(NULL, buf, CSIDL_LOCAL_APPDATA, FALSE);
			m_datadirectory = buf;
			m_datadirectory.append("\\");
			m_datadirectory.append(_uplugin_identifier);

		} else if (osVersionInfo.dwMajorVersion > 5) {
			// Vista
			// TODO @@@ SHGetSpecialFolderPath
		} else {
			// TODO @@@ log "Unsupported windows version"
		}
#endif
	}
	return m_datadirectory;
}

const string &Env::getAppBaseDirectory() const {
	if (m_appbasedirectory.empty()) {
		m_appbasedirectory = this->getDataDirectory();
		m_appbasedirectory.append("\\");
		m_appbasedirectory.append("App");
	}
	return m_appbasedirectory;
}

const string &Env::getDotNetSupportDLL() const {
	if (m_dotnetsupportdll.empty()) {
		m_dotnetsupportdll = this->getInstallDirectory();
		m_dotnetsupportdll.append("\\DotNetSupport\\");
		m_dotnetsupportdll.append("npUPlugin.DotNetSupport.dll");
	}
	return m_dotnetsupportdll;
}