#include <shlobj.h>
#include <sstream>
#include "UPlugin/Env.h"
#include "Poco/SharedLibrary.h"
#include "Poco/Path.h"
#include "Poco/File.h"

using namespace std;
using namespace UPlugin;
using namespace Poco;
using namespace Poco::Util;

static const std::string _uplugin_identifier = "UPlugin";
//static const std::string _base_directory = "E:\\UPlugin\\git_repo\\bin";
static const std::string _app_dir_name = "App";
static const std::string _dotnetsupport_dll_name = "npUPlugin.DotNetSupport.dll";

Env::Env(void)
{
	m_base_user_dir = ".";
	//m_base_user_low_dir = ".";
	m_base_prog_dir = ".";

	OSVERSIONINFO osVersionInfo;
	osVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVersionInfo);

	if (osVersionInfo.dwMajorVersion == 5) {
		// XP
		// User AppLocalData Folder
		{
			char buf[MAX_PATH];
			SHGetSpecialFolderPath(NULL, buf, CSIDL_LOCAL_APPDATA, FALSE);
			m_base_user_dir = buf;
			//m_base_user_low_dir = m_base_user_dir;
		}
		// ProgramFiles Folder
		{
			char buf[MAX_PATH];
			SHGetSpecialFolderPath(NULL, buf, CSIDL_PROGRAM_FILES, FALSE);
			m_base_prog_dir = buf;
		}
	} else if (osVersionInfo.dwMajorVersion > 5) {
		// Vista
		typedef HRESULT ( WINAPI *FPSHGetKnownFolderPath )( const GUID&, DWORD, HANDLE, PWSTR * );
		FPSHGetKnownFolderPath func;
		Poco::SharedLibrary lib("shell32.dll");
		if (lib.hasSymbol("SHGetKnownFolderPath")) {
			func = (FPSHGetKnownFolderPath) lib.getSymbol("SHGetKnownFolderPath");
		} else {
			// TODO @@ log
			return;
		}
		// User AppLocalData Folder
		/*
		{
			char buf[MAX_PATH];
			PWSTR pstr = NULL;
			HRESULT hr = ( *func )( FOLDERID_LocalAppDataLow, 0, NULL, &pstr );
			int len = WideCharToMultiByte(CP_ACP, 0, pstr, -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, pstr, wcslen(pstr)+1, buf, len, NULL, NULL);
			m_base_user_dir = buf;
		}
		*/
		{
			char buf[MAX_PATH];
			PWSTR pstr = NULL;
			HRESULT hr = ( *func )( FOLDERID_LocalAppData, 0, NULL, &pstr );
			int len = WideCharToMultiByte(CP_ACP, 0, pstr, -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, pstr, wcslen(pstr)+1, buf, len, NULL, NULL);
			m_base_user_dir = buf;
		}
		// ProgramFiles Folder
		{
			char buf[MAX_PATH];
			PWSTR pstr = NULL;
			HRESULT hr = ( *func )( FOLDERID_ProgramFiles, 0, NULL, &pstr );
			int len = WideCharToMultiByte(CP_ACP, 0, pstr, -1, NULL, 0, NULL, NULL);
			WideCharToMultiByte(CP_ACP, 0, pstr, wcslen(pstr)+1, buf, len, NULL, NULL);
			m_base_prog_dir = buf;
		}
	} else {
		// TODO @@@ log "Unsupported windows version"
		// .. can not log here, logger is not inited yet
	}

	// UPlugin.ini - Just for Debug
	m_ini_conf_file = m_base_user_dir;
	m_ini_conf_file.append("\\");
	m_ini_conf_file.append(_uplugin_identifier);
	m_ini_conf_file.append("\\");
	m_ini_conf_file.append(_uplugin_identifier + ".ini");

	try {
		// load DIR configuration from ini
		m_config = new IniFileConfiguration(m_ini_conf_file);
		{
			std::string conf_dir = m_config->getString("Install_DIR");
			if (!conf_dir.empty()) {
				m_installdirectory = conf_dir;
			}
		}
		{
			std::string conf_dir = m_config->getString("Data_DIR");
			if (!conf_dir.empty()) {
				m_datadirectory = conf_dir;
			}
		}
	} catch (...) {
		m_config = NULL;
		// @@@ nothing happens
	}

	// UPlugin install DIR C:/Program Files/UPlugin
	if (m_installdirectory.empty()) {
		m_installdirectory = m_base_prog_dir;
		m_installdirectory.append("\\");
		m_installdirectory.append(_uplugin_identifier);
	}

	// User Data DIR C:/[UserHomePath]/[LoadAppDataPath]/UPlugin
	if (m_datadirectory.empty()) {
		m_datadirectory = m_base_user_dir;
		m_datadirectory.append("\\");
		m_datadirectory.append(_uplugin_identifier);
	}

	m_appbasedirectory = m_datadirectory;
	m_appbasedirectory.append("\\");
	m_appbasedirectory.append(_app_dir_name);

	// TODO @@@ use native C++
	{ // create data directory if not exists
		Poco::Path p(m_datadirectory);
		Poco::File f(p);
		if (!f.exists()) {
			f.createDirectory();
		}
	}
	{ // create appbase directory if not exists
		Poco::Path p(m_datadirectory, _app_dir_name);
		Poco::File f(p);
		if (!f.exists()) {
			f.createDirectory();
		}
	}

	/*
	System::String ^mgd_data_dir = gcnew System::String(m_datadirectory.c_str());
	Poco::Path::
	if (!System::IO::Directory::Exists(mgd_data_dir)) {
		System::IO::Directory::CreateDirectory(mgd_data_dir);
	}
	// UPlugin managed APPs C:/[UserHomePath]/[LoadAppDataPath]/UPlugin/App
	m_appbasedirectory = m_datadirectory;
	m_appbasedirectory.append("\\");
	m_appbasedirectory.append(_app_dir_name);
	// TODO @@@ use native C++
	System::String ^mgd_app_dir = gcnew System::String(m_appbasedirectory.c_str());
	if (!System::IO::Directory::Exists(mgd_app_dir)) {
		System::IO::Directory::CreateDirectory(mgd_app_dir);
	}
	*/

	// UPlugin DotNetSupprt DLL C:/Program Files/UPlugin/UPlugin.DotNetSupport.dll
	m_dotnetsupportdll = m_installdirectory;
	m_dotnetsupportdll.append("\\");
	m_dotnetsupportdll.append(_dotnetsupport_dll_name);
}

Env::~Env(void)
{
}

const string &Env::getInstallDirectory() const
{
	return m_installdirectory;
}

const string &Env::getDataDirectory() const
{
	return m_datadirectory;
}

const string &Env::getAppBaseDirectory() const {
	return m_appbasedirectory;
}

const string &Env::getDotNetSupportDLL() const {
	return m_dotnetsupportdll;
}

const string &Env::getBaseUserDIR() const {
	return m_base_user_dir;
};

const string &Env::getBaseProgramDIR() const {
	return m_base_prog_dir;
};

const string &Env::getConfigFile() const {
	return m_ini_conf_file;
};

const IniFileConfiguration *Env::getConfig() const {
	return m_config.get();
}