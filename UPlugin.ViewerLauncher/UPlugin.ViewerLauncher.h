#pragma once

#include <map>
#include <string>
#include "UPlugin/ScriptableObject.h"

using namespace UPlugin;

class ViewerLauncherPlugin : public ScriptablePluginObject<ViewerLauncherPlugin>
{
private:
	std::string m_viewer_exe_path;

public:
	ViewerLauncherPlugin();
	ViewerLauncherPlugin(NPP npp);
	virtual ~ViewerLauncherPlugin();
	virtual void shutdown();

	bool launch(const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool getInstalledViewers(const NPVariant *args, uint32_t argCount, NPVariant *result);
	bool close(const NPVariant *args, uint32_t argCount, NPVariant *result);

private:
	std::vector<std::string> m_supportedviewers;
	std::vector<std::string> m_installedviewers;
	std::string getViewerPath(const std::string &name);
	bool isInstalled(const std::string &name);

};

DECLARE_UPLUGIN_APP_CLASS(ViewerLauncherPlugin);
