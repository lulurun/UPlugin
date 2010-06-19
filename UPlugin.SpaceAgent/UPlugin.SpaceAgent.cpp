#include "UPlugin/Env.h"
#include "UPlugin.SpaceAgent.h"

using namespace System;

static const std::string APP_EXE_NAME = "SpaceApp.exe";
static const std::string PROCESS_NAME = "SpaceApp";
static const std::string SHUTDOWN_URL = "http://127.0.0.1:7475/AgentService/shutdown";
static const std::string LOADCONTENTS_URL = "http://127.0.0.1:7475/AgentService/loadContents";
static const std::string GETSTATUS_URL = "http://127.0.0.1:7475/AgentService/status";

enum LocalProcessStatus{
	STARTED = -1,
	NOT_STARTED = -2,
	DUPLICATE = -3,
};

static char* np_strdup(const char *s) {
	if ((const char *) 0 == s )
		s = "";
	size_t n = strlen(s) + 1;
	char *rv = (char *) Util::MemAlloc(n);
	if ((char *) 0 != rv )
		memcpy(rv, s, n);
	return rv;
}

SpaceAgentPlugin::SpaceAgentPlugin()
:ScriptablePluginObject()
{
}

SpaceAgentPlugin::SpaceAgentPlugin(NPP npp)
	:ScriptablePluginObject(npp)
{
	m_identifier = "SpaceAgent";

	// default settings
	m_app_exe_path = Env::GetInstance()->getAppBaseDirectory();
	m_app_exe_path += "\\";
	m_app_exe_path += m_identifier;
	m_app_exe_path += "\\bin\\";
	m_app_exe_path += APP_EXE_NAME;
	m_shutdown_url = SHUTDOWN_URL;
	m_loadcontents_url = LOADCONTENTS_URL;
	m_getstatus_url = GETSTATUS_URL;
	m_run_foreground = false;

	const Poco::Util::IniFileConfiguration *config = Env::GetInstance()->getConfig();
	if (config != NULL) {
		{
			std::string conf_res = config->getString(m_identifier + ".AppExePath");
			if (!conf_res.empty()){ 
				m_app_exe_path = conf_res;
			}
		}
		{
			std::string conf_res = config->getString(m_identifier + ".Shutdown_URL");
			if (!conf_res.empty()) {
				m_shutdown_url = conf_res;
			}
		}
		{
			std::string conf_res = config->getString(m_identifier + ".LoadContents_URL");
			if (!conf_res.empty()) {
				m_loadcontents_url = conf_res;
			}
		}
		{
			std::string conf_res = config->getString(m_identifier + ".GetStatus_URL");
			if (!conf_res.empty()) {
				m_getstatus_url = conf_res;
			}
		}
		{
			bool conf_res = config->getBool(m_identifier + ".RunForeground", false);
			m_run_foreground = conf_res;
		}
	}
	register_method("Run", this, &SpaceAgentPlugin::runSpace);
	register_method("Stop", this, &SpaceAgentPlugin::shutdownSpace);
	register_method("LoadContents", this, &SpaceAgentPlugin::loadContents);
	register_method("GetStatus", this, &SpaceAgentPlugin::getStatus);
}

SpaceAgentPlugin::~SpaceAgentPlugin()
{
}

bool SpaceAgentPlugin::runSpace(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	VOID_TO_NPVARIANT(*result);
	try {
		int local_res = this->getLocalProcessStatus();
		if (local_res == STARTED) {
			// TODO @@@ log
			BOOLEAN_TO_NPVARIANT(false, *result);
			return true;
		} else if (local_res == DUPLICATE) {
			// TODO @@@ log
			this->forceStopAll();
			BOOLEAN_TO_NPVARIANT(false, *result);
			return true;
		}
		System::Diagnostics::ProcessStartInfo ^psi = gcnew System::Diagnostics::ProcessStartInfo();
		System::String ^managed_app_exe_path = gcnew String(m_app_exe_path.c_str());
		psi->FileName = managed_app_exe_path;
		psi->WorkingDirectory = System::IO::Path::GetDirectoryName(managed_app_exe_path);
		if(!m_run_foreground) {
			psi->CreateNoWindow = true;
			psi->UseShellExecute = false;
		}
		System::Diagnostics::Process::Start(psi);
		BOOLEAN_TO_NPVARIANT(true, *result);
	} catch (System::Exception ^e) {
		// TODO @@@ log
		return false;
	}
	return true;
}

bool SpaceAgentPlugin::shutdownSpace(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	VOID_TO_NPVARIANT(*result);
	try {
		bool res = this->_shutdownSpace();
		BOOLEAN_TO_NPVARIANT(res, *result);
	} catch (System::Exception ^e) {
		// TODO @@@ log
		BOOLEAN_TO_NPVARIANT(false, *result);
		return false;
	}
	return true;
}

bool SpaceAgentPlugin::_shutdownSpace() {
	int local_res = this->getLocalProcessStatus();
	if (local_res == NOT_STARTED) {
		// TODO @@@ log
		return false;
	} else if (local_res == DUPLICATE) {
		// TODO @@@ log
		this->forceStopAll();
		return false;
	}
	String ^url = gcnew String(m_shutdown_url.c_str());
	Net::WebClient ^wc = gcnew Net::WebClient();
	String ^res = wc->DownloadString(url);
	if (res->Contains("true")) {
		return true;
	} else {
		return false;
	}
}

bool SpaceAgentPlugin::loadContents(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	if (argCount != 1) return false;
	if (!NPVARIANT_IS_STRING(args[0])) return false;

	VOID_TO_NPVARIANT(*result);

	try {
		int local_res = this->getLocalProcessStatus();
		if (local_res == NOT_STARTED) {
			// TODO @@@ log
			BOOLEAN_TO_NPVARIANT(false, *result);
			return true;
		} else if (local_res == DUPLICATE) {
			// TODO @@@ log
			this->forceStopAll();
			BOOLEAN_TO_NPVARIANT(false, *result);
			return true;
		}

		String^ content_url = gcnew String(Util::NPVariantToString(args[0]).c_str());
		String ^service_url = gcnew String(m_loadcontents_url.c_str());
		String ^url = service_url + "?url=" + content_url;

		Net::WebClient ^wc = gcnew Net::WebClient();
		String ^res = wc->DownloadString(url);
		if (res->Contains("true")) {
			BOOLEAN_TO_NPVARIANT(true, *result);
		} else {
			BOOLEAN_TO_NPVARIANT(false, *result);
		}
	} catch (System::Exception ^e) {
		// TODO @@@ log
		return false;
	}
	return true;
}

bool SpaceAgentPlugin::getStatus(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	bool getRemoteRes = false;
	if (argCount == 1 && NPVARIANT_IS_BOOLEAN(args[0])) {
		Util::NPVariantParseBoolean(args[0], &getRemoteRes);
	}
	VOID_TO_NPVARIANT(*result);
	try {
		int local_res = this->getLocalProcessStatus();
		if (local_res != STARTED) {
			INT32_TO_NPVARIANT(local_res, *result);
			return true;
		}
		if (!getRemoteRes) {
			INT32_TO_NPVARIANT(local_res, *result);
		} else {
			String ^url = gcnew String(m_getstatus_url.c_str());
			Net::WebClient ^wc = gcnew Net::WebClient();
			String ^res = wc->DownloadString(url);
			int res_int = System::Convert::ToInt32(res);
			INT32_TO_NPVARIANT(res_int, *result);
		}
	} catch (System::Exception ^e) {
		// TODO @@@ log
		return false;
	}
	return true;
}

int SpaceAgentPlugin::getLocalProcessStatus() {
	bool process_started = false;
	bool process_duplicate = false;
	cli::array<System::Diagnostics::Process ^> ^ps =
		System::Diagnostics::Process::GetProcessesByName(gcnew String(PROCESS_NAME.c_str()));
	if (ps != nullptr) {
		if (ps->Length == 1) {
			process_started = true;
		}
		if (ps->Length > 1) {
			process_duplicate = true;
		}
	}
	if (process_started) return STARTED;
	if (process_duplicate) return DUPLICATE;
	return NOT_STARTED;
}

void SpaceAgentPlugin::forceStopAll() {
	cli::array<System::Diagnostics::Process ^> ^ps =
		System::Diagnostics::Process::GetProcessesByName(gcnew String(PROCESS_NAME.c_str()));
	if (ps != nullptr) {
		for each(System::Diagnostics::Process ^p in ps) {
			p->Kill();
		}
	}

}

void SpaceAgentPlugin::shutdown() {
	//this->_shutdownSpace();
}

