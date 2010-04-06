#pragma once

namespace UPlugin {
namespace DotNet {

ref class DotNetEnv
{
public:

	static System::String ^Identifier = "XXX";

private:

	static System::String ^_DIR_UPluginSoftware = System::IO::Path::Combine(System::Environment::GetFolderPath(System::Environment::SpecialFolder::LocalApplicationData), "UPlugin");
	static System::String ^_DIR_App = System::IO::Path::Combine(DIR_UPluginSoftware, Identifier);
	static System::String ^_DIR_Installation = System::IO::Path::Combine(_DIR_App, "bin");
	static System::String ^_DIR_Download_TMP = System::IO::Path::Combine(_DIR_App, "." + Identifier + ".tmp");
	static System::String ^_DIR_Install_TMP = DIR_Installation + ".old";
	static System::String ^_FILE_PKG_Download = System::IO::Path::Combine(DIR_App, "." + Identifier +".pkg");
	static System::String ^_FILE_LastUpdate = System::IO::Path::Combine(DIR_App, ".last.upd");
	static System::String ^_FILE_VersionInfo_Cache = System::IO::Path::Combine(DIR_App, ".ver.cache");
	static System::String ^_DIR_GUI = System::IO::Path::Combine(DIR_App, "gui");

	static System::String ^_FILE_BrowserPlugin_Log = System::IO::Path::Combine(DIR_App, "logs\\bp.log");
	static System::String ^_FILE_keepalive = System::IO::Path::Combine(DIR_App, ".keepalive");

public:

#ifdef _DEBUG
	static System::String ^Default_Update_URL = "";
	static int Default_Check_Update_Interval = 60 * 5;
#else
	static System::String ^Default_Update_URL = "";
	static int Default_Check_Update_Interval = 86400; // daily
#endif

	static property System::String ^DIR_UPluginSoftware { System::String ^get() { return _DIR_UPluginSoftware; } }
	static property System::String ^DIR_App { System::String ^get() { return _DIR_App; } }
	static property System::String ^DIR_Installation { System::String ^get() { return _DIR_Installation; } }
	static property System::String ^DIR_Download_TMP { System::String ^get() { return _DIR_Download_TMP; } }
	static property System::String ^DIR_Install_TMP { System::String ^get() { return _DIR_Install_TMP; } }
	static property System::String ^FILE_PKG_Download { System::String ^get() { return _FILE_PKG_Download; } }
	static property System::String ^FILE_LastUpdate { System::String ^get() { return _FILE_LastUpdate; } }
	static property System::String ^FILE_VersionInfo_Cache { System::String ^get() { return _FILE_VersionInfo_Cache; } }
	static property System::String ^DIR_GUI { System::String ^get() { return _DIR_GUI; } }

	static property bool Installed { bool get() { return System::IO::File::Exists(DIR_App); } }

	static property System::String ^FILE_BrowserPlugin_Log { System::String ^get() { return _FILE_BrowserPlugin_Log; } }
	static property System::String ^FILE_keepalive { System::String ^get() { return _FILE_keepalive; } }

	static property System::String ^LockName { System::String ^get() { return "." + Identifier + ".lock"; } }
};

}
}

