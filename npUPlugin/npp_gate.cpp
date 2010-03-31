#include "plugin.h"

using namespace UPlugin;

char*NPP_GetMIMEDescription(void)
{
	return "application/x-uplugin";
}

NPError NPP_Initialize(void)
{
	return NPERR_NO_ERROR;
}

void NPP_Shutdown(void)
{
}

// here the plugin creates an instance of our nsPluginInstance object which 
// will be associated with this newly created plugin instance and 
// will do all the neccessary job
NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16_t mode,
				int16_t argc, char* argn[], char* argv[], NPSavedData* saved)
{
	if(instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	NPError rv = NPERR_NO_ERROR;

	Plugin *plugin = new Plugin(instance);
	if(plugin == NULL)
		return NPERR_OUT_OF_MEMORY_ERROR;
	plugin->onNPP_New(argc, argn, argv);
	instance->pdata = plugin;

	return rv;
}

// here is the place to clean up and destroy the nsPluginInstance object
NPError NPP_Destroy(NPP instance, NPSavedData** save)
{
	return NPERR_NO_ERROR;
	if(instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	NPError rv = NPERR_NO_ERROR;

	Plugin *plugin = (Plugin *)instance->pdata;
	if(plugin != NULL) {
		plugin->onNPP_Destroy();
		delete plugin;
	}
	return rv;
}

// during this call we know when the plugin window is ready or
// is about to be destroyed so we can do some gui specific
// initialization and shutdown
NPError NPP_SetWindow(NPP instance, NPWindow* pNPWindow)
{
	if(instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	NPError rv = NPERR_NO_ERROR;

	if(pNPWindow == NULL)
		return NPERR_GENERIC_ERROR;

	Plugin *plugin = (Plugin *)instance->pdata;

	if(plugin == NULL)
		return NPERR_GENERIC_ERROR;

	// window just created
	if(!plugin->isWindowSet() && (pNPWindow->window != NULL)) {
		if(!plugin->onNPP_SetWindows(pNPWindow)) {
			delete plugin;
			plugin = NULL;
			return NPERR_MODULE_LOAD_FAILED_ERROR;
		}
	}

	// window goes away
	if((pNPWindow->window == NULL) && plugin->isWindowSet())
		return NPERR_NO_ERROR;

	// window resized
	if(plugin->isWindowSet() && (pNPWindow->window != NULL))
		return NPERR_NO_ERROR;

	// this should not happen, nothing to do
	if((pNPWindow->window == NULL) && !plugin->isWindowSet())
		return NPERR_NO_ERROR;

	return rv;
}

// ==============================
// ! Scriptability related code !
// ==============================
//
// here the plugin is asked by Mozilla to tell if it is scriptable
// we should return a valid interface id and a pointer to 
// nsScriptablePeer interface which we should have implemented
// and which should be defined in the corressponding *.xpt file
// in the bin/components folder
NPError	NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
	if(instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	Plugin *plugin = (Plugin *)instance->pdata;
	if(plugin == NULL)
		return NPERR_GENERIC_ERROR;

	if (!plugin->onNPP_GetValue(variable, value)) {
		return NPERR_GENERIC_ERROR;
	}
	return NPERR_NO_ERROR;
}

NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream* stream, NPBool seekable, uint16_t* stype)
{
	if(instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	Plugin *plugin = (Plugin *)instance->pdata;
	if(plugin == NULL)
		return NPERR_GENERIC_ERROR;

	if (!plugin->onNPP_NewStream(type, stream, seekable, stype)) {
		return NPERR_GENERIC_ERROR;
	}
	return NPERR_NO_ERROR;
}

int32_t NPP_WriteReady (NPP instance, NPStream *stream)
{
	if(instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	Plugin *plugin = (Plugin *)instance->pdata;
	if(plugin == NULL)
		return NPERR_GENERIC_ERROR;

	return plugin->onNPP_WriteReady(stream);
}

int32_t NPP_Write (NPP instance, NPStream *stream, int32_t offset, int32_t len, void *buffer)
{   
	if(instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	Plugin *plugin = (Plugin *)instance->pdata;
	if(plugin == NULL)
		return NPERR_GENERIC_ERROR;

	return plugin->onNPP_Write(stream, offset, len, buffer);
}

NPError NPP_DestroyStream (NPP instance, NPStream *stream, NPError reason)
{
	if(instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	Plugin *plugin = (Plugin *)instance->pdata;
	if(plugin == NULL)
		return NPERR_GENERIC_ERROR;

	if (!plugin->onNPP_DestroyStream(stream, reason)) {
		return NPERR_GENERIC_ERROR;
	}
	return NPERR_NO_ERROR;
}

void NPP_StreamAsFile (NPP instance, NPStream* stream, const char* fname)
{
	if(instance == NULL)
		return; // TODO @@@ log

	Plugin *plugin = (Plugin *)instance->pdata;
	if(plugin == NULL)
		return; // TODO @@@ log

	return plugin->onNPP_StreamAsFile(stream, fname);
}

void NPP_Print (NPP instance, NPPrint* printInfo)
{
	if(instance == NULL)
		return; // TODO @@@ log

	Plugin *plugin = (Plugin *)instance->pdata;
	if(plugin == NULL)
		return; // TODO @@@ log

	return plugin->onNPP_Print(printInfo);
}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
	if(instance == NULL)
		return;
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
	if(instance == NULL)
		return NPERR_INVALID_INSTANCE_ERROR;

	Plugin *plugin = (Plugin *)instance->pdata;
	if(plugin == NULL)
		return NPERR_GENERIC_ERROR; // TODO @@@ log

	if (!plugin->onNPP_SetValue(variable, value)) {
		return NPERR_GENERIC_ERROR;
	}
	return NPERR_NO_ERROR;
}

int16_t	NPP_HandleEvent(NPP instance, void* event)
{
	if(instance == NULL)
		return 0;

	int16_t rv = 0;
	Plugin *plugin = (Plugin *)instance->pdata;
	if (plugin)
		rv = plugin->onNPP_HandleEvent(event);

	return rv;
}

NPObject *NPP_GetScriptableInstance(NPP instance)
{
	if(!instance)
		return NULL; // TODO @@@ log

	NPObject *npobj = NULL;
	Plugin * plugin = (Plugin *)instance->pdata;
	if (!plugin)
		npobj = plugin->onNPP_GetScriptableInstance(); // TODO @@@ log when plugin == NULL;

	return npobj;
}
