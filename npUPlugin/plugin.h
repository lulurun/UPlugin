#ifndef __UPLUGIN_PLUGIN_H__
#define __UPLUGIN_PLUGIN_H__

#include <map>
#include <string>
#include "UPlugin/ScriptableBase.h"

namespace UPlugin {

class Plugin {

public:

	Plugin(NPP);
	~Plugin();

	// NPP Entries
	bool onNPP_New(int16_t argc, char* argn[], char* argv[]);
	bool onNPP_Destroy();
	bool onNPP_SetWindows(NPWindow* pNPWindow);
	bool onNPP_GetValue(NPPVariable variable, void *value);
	bool onNPP_NewStream(NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype);
	int32_t onNPP_WriteReady (NPStream *stream);
	int32_t onNPP_Write (NPStream *stream, int32_t offset, int32_t len, void *buffer);
	bool onNPP_DestroyStream (NPStream *stream, NPError reason);
	void onNPP_StreamAsFile (NPStream* stream, const char* fname);
	void onNPP_Print (NPPrint* printInfo);
	void onNPP_URLNotify(const char* url, NPReason reason, void* notifyData);
	bool onNPP_SetValue(NPNVariable variable, void *value);
	int16_t	onNPP_HandleEvent(void* event);
	NPObject *onNPP_GetScriptableInstance();

	//const NPNetscapeFuncs* getBrowser() { return &GetNPNFuncs(); }
	NPP getNPP() const { return m_npp; }
	const bool isWindowSet() const { return m_windowset; }

private:

	NPObject *getScriptableObject();

private:

	NPP m_npp;
	ScriptablePluginObjectBase* m_scriptable;
	bool m_windowset;
	HWND mhWnd;

	// app settings
	std::string m_app_name;
	std::string m_app_dllname;
	std::string m_app_requiredversion;
	std::string m_app_updateurl;

	typedef std::map<std::string, std::string> PARAMLIST_TYPE;
	PARAMLIST_TYPE m_param_list;
};

};

#endif // __UPLUGIN_PLUGIN_H__
