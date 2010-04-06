#include "UPlugin/ScriptableBase.h"
#include "Plugin.h"
#include "AppManager.h"

using namespace UPlugin;

Plugin::Plugin(NPP npp)
	:m_npp(npp), m_scriptable(NULL), m_windowset(false), mhWnd(NULL)
{
}

Plugin::~Plugin() {
	if (this->m_scriptable) {
		//this->_scriptable->pluginDeath(); //TODO @@@ ???
		NPN_ReleaseObject(this->m_scriptable);
		this->m_scriptable = NULL;
	}
}

bool Plugin::onNPP_New(int16_t argc, char* argn[], char* argv[]) {
	for(int16_t i=0; i<argc; i++) {
		std::string n(argn[i]);
		std::string v;
		if (argv[i] != NULL) {
			v = argv[i];
		}
		this->m_param_list.insert(PARAMLIST_TYPE::value_type(n, v));

		if (n == "UPlugin.Name") {
			this->m_app_name = v;
		}
		if (n == "UPlugin.DLLName") {
			this->m_app_dllname = v;
		}
		if (n == "UPlugin.UpdateUrl") {
			this->m_app_updateurl = v;
		}
		if (n == "UPlugin.RequiredVersion") {
			this->m_app_requiredversion = v;
		}
		if (n == "UPlugin.DebugMode") {
			std::string value(v);
			if (value == "on") {
				//MessageBox(NULL, "DEBUG START", "UPLUGIN", MB_OK);
			}
		}
	}
	return true;
}

bool Plugin::onNPP_Destroy() {
	// TODO @@@ shutdown DotNetInstance
	this->m_windowset = false;
	return true;
}

bool Plugin::onNPP_SetWindows(NPWindow* pNPWindow) {
	if(pNPWindow == NULL || pNPWindow->window == NULL) return false;

	ScriptablePluginObjectBase *scriptable = (ScriptablePluginObjectBase *)this->getScriptableObject();
	if (scriptable->setWindowBase(pNPWindow)) {
		m_windowset = true;
		return true;
	}
	return false;
}

NPObject *Plugin::getScriptableObject() {
	if (!m_scriptable) {
		m_scriptable = m_app_mgr.createScriptable(m_npp, m_app_name, m_app_dllname);
	}

	if (m_scriptable) {
		NPN_RetainObject(m_scriptable);
	}
	return m_scriptable;
}

bool Plugin::onNPP_GetValue(NPPVariable variable, void *value) {
	// TODO @@@ get values from m_scriptable
	switch (variable) {
	case NPPVpluginNameString:
		*((char **)value) = "UPlugin";
		break;
	case NPPVpluginDescriptionString:
		*((char **)value) = "UPlugin for NPRuntime";
		break;
	case NPPVpluginScriptableNPObject:
		*(NPObject **)value = this->getScriptableObject();
		break;
	default:
		return false;
	}
	return true;
}

bool Plugin::onNPP_NewStream(NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype) {
	return true;
}

int32_t Plugin::onNPP_WriteReady (NPStream *stream) {
	int32_t rv = 0x0fffffff;
	return rv;
}

int32_t Plugin::onNPP_Write (NPStream *stream, int32_t offset, int32_t len, void *buffer) {
	int32_t rv = len;
	return rv;
}

bool Plugin::onNPP_DestroyStream (NPStream *stream, NPError reason) {
	return true;
}

void Plugin::onNPP_StreamAsFile (NPStream* stream, const char* fname) {
	return;
}

void Plugin::onNPP_Print (NPPrint* printInfo) {
	return;
}

void Plugin::onNPP_URLNotify(const char* url, NPReason reason, void* notifyData) {
	return;
}

bool Plugin::onNPP_SetValue(NPNVariable variable, void *value) {
	return true;
}

int16_t	Plugin::onNPP_HandleEvent(void* event) {
	// TODO @@ not impled
	return 0;
}

NPObject *Plugin::onNPP_GetScriptableInstance() {
	return this->getScriptableObject();
}
