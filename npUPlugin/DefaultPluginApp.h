#ifndef __DEFAULTPLUGINAPP_H__
#define __DEFAULTPLUGINAPP_H__

#include "UPlugin/ScriptableBase.h"

namespace UPlugin {

class DefaultPluginApp : public ScriptablePluginObjectBase
{
public:
	DefaultPluginApp(NPP npp);
	virtual ~DefaultPluginApp();

	virtual bool HasMethod(NPIdentifier name);
	virtual bool Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result);
};

static NPObject *AllocateDefaultPluginAppObject(NPP npp, NPClass *aClass)
{
	return new DefaultPluginApp(npp);
}

DECLARE_NPOBJECT_CLASS_WITH_BASE(DefaultPluginApp, AllocateDefaultPluginAppObject);

};

#endif // __DEFAULTPLUGINAPP_H__

