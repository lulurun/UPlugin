#pragma once

#include <string>
#include "UPlugin/ScriptableObject.h"

using namespace UPlugin;

class SpaceAgentPlugin : public ScriptablePluginObject<SpaceAgentPlugin>
{
private:
	// config members
	std::string m_app_exe_path;
	std::string m_shutdown_url;
	std::string m_loadcontents_url;
	std::string m_getstatus_url;
	bool m_run_foreground;

public:
	SpaceAgentPlugin();
	SpaceAgentPlugin(NPP npp);
	virtual ~SpaceAgentPlugin();

	bool runSpace(const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool shutdownSpace(const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool loadContents(const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool getStatus(const NPVariant *args, uint32_t argCount, NPVariant *result);

	virtual void shutdown();

private:
	int getLocalProcessStatus();
	void forceStopAll();

	bool _shutdownSpace();
};

DECLARE_UPLUGIN_APP_CLASS(SpaceAgentPlugin);
