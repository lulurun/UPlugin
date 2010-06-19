#include <string>
#include "UPlugin/Env.h"
#include "UPlugin/Logger.h"
#include "UPlugin.ViewerLauncher.h"

#include "Poco/Path.h"
#include "Poco/File.h"
#include "Poco/Format.h"
#include "Poco/Process.h"

using namespace System;
using namespace UPlugin;

static const std::string APP_EXE_NAME = "realXtend.exe";
static const std::string PROCESS_NAME = "realXtend";

static char* np_strdup(const char *s) {
	if ((const char *) 0 == s ) s = "";
	size_t n = strlen(s) + 1;
	char *rv = (char *) Util::MemAlloc(n);
	if ((char *) 0 != rv ) memcpy(rv, s, n);
	return rv;
}


ViewerLauncherPlugin::ViewerLauncherPlugin()
:ScriptablePluginObject()
{
}

ViewerLauncherPlugin::ViewerLauncherPlugin(NPP npp)
	:ScriptablePluginObject(npp)
{
	m_identifier = "ViewerLauncher";

	m_supportedviewers.push_back("SecondLife");
	m_supportedviewers.push_back("SecondLife2");
	m_supportedviewers.push_back("RealXtend");
	m_supportedviewers.push_back("RealXtend 3Di");
	// get installed viewer
	for(std::vector<std::string>::iterator it=m_supportedviewers.begin(); it!=m_supportedviewers.end(); it++) {
		if (this->isInstalled(*it)) {
			m_installedviewers.push_back(*it);
		}
	}
	// default settings
	register_method("ViewerList", this, &ViewerLauncherPlugin::getInstalledViewers);
	register_method("Launch", this, &ViewerLauncherPlugin::launch);
	register_method("Close", this, &ViewerLauncherPlugin::close);
}

ViewerLauncherPlugin::~ViewerLauncherPlugin()
{
}

void ViewerLauncherPlugin::shutdown() {
}

std::string ViewerLauncherPlugin::getViewerPath(const std::string &name) {
	Poco::Path p(Env::GetInstance()->getBaseProgramDIR());
	if (name == "SecondLife") {
		p.append("SecondLife");
		p.append("SecondLife.exe");
	} else if (name == "SecondLife2") {
		p.append("SecondLifeViewer2");
		p.append("SecondLife.exe");
	} else if (name == "RealXtend") {
		p.append("RealXtend");
		p.append("RealXtend.exe");
	} else if (name == "RealXtend 3Di") {
		p.append("RealXtend_3Di");
		p.append("RealXtend.exe");
	} else {
		p.clear();
	}
	return p.toString();
}

bool ViewerLauncherPlugin::isInstalled(const std::string &name) {
	std::string path = this->getViewerPath(name);
	Poco::File f(path);
	return f.exists();
}

bool ViewerLauncherPlugin::getInstalledViewers(const NPVariant *args, uint32_t argCount, NPVariant *result) {	
	std::string ret_json = "[";
	for(std::vector<std::string>::iterator it=m_installedviewers.begin(); it!=m_installedviewers.end(); it++) {
		ret_json.append("'").append(*it).append("',");
	}
	ret_json.append("]");
	STRINGZ_TO_NPVARIANT(np_strdup(ret_json.c_str()), *result);
	return true;
}

bool ViewerLauncherPlugin::launch(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	BOOLEAN_TO_NPVARIANT(false, *result);
	if (argCount != 6) return false;
	std::string viewer(args[0].value.stringValue.UTF8Characters);
	if (!this->isInstalled(viewer)) {
		ERROR_LOG("Launch: " << viewer << " not installed");
		return true;
	}
	std::string viewer_exe_path = this->getViewerPath(viewer);
	std::string first(args[1].value.stringValue.UTF8Characters);
	std::string last(args[2].value.stringValue.UTF8Characters);
	std::string password(args[3].value.stringValue.UTF8Characters);
	std::string loginserver(args[4].value.stringValue.UTF8Characters);
	std::string location(args[5].value.stringValue.UTF8Characters);
	// --login * * * --loginuri ' + gSpaceApp.LoginServerURL + '?uri:' + client_id + '&128&128&128:10001'
	std::string loginuri = Poco::format("%s?%s", loginserver, location);

#if USE_POCO_PROCESS
	Poco::Process::Args rex_args;
	rex_args.push_back("--login");
	rex_args.push_back(first);
	rex_args.push_back(last);
	rex_args.push_back(password);
	rex_args.push_back("--loginuri");
	rex_args.push_back(loginuri);
	//Poco::ProcessHandle ph = Process::launch(m_rex_exe_path, rex_args);
	Poco::Process::launch(m_rex_exe_path, rex_args);
#else
	std::string viewer_args = Poco::format("--login %s %s %s --loginuri %s", first, last, password, loginuri);
	INFO_LOG("Launch: " << viewer_exe_path << " " << viewer_args);

	try {
		System::String ^managed_args = gcnew System::String(viewer_args.c_str());
		System::Diagnostics::ProcessStartInfo ^psi = gcnew System::Diagnostics::ProcessStartInfo();
		System::String ^managed_exe_path = gcnew String(viewer_exe_path.c_str());
		psi->FileName = managed_exe_path;
		psi->Arguments = managed_args;
		psi->WorkingDirectory = System::IO::Path::GetDirectoryName(managed_exe_path);
		/*
		psi->CreateNoWindow = true;
		psi->UseShellExecute = false;
		*/
		System::Diagnostics::Process::Start(psi);
	} catch (Exception ^e) {
		// TODO @@@ log ??
		return true;
	}
#endif
	BOOLEAN_TO_NPVARIANT(true, *result);
	return true;
}

bool ViewerLauncherPlugin::close(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	VOID_TO_NPVARIANT(*result);
	return true;
}

