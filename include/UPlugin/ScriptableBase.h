#ifndef __UPLUGIN_SCRIPTABLEBASE_H__
#define __UPLUGIN_SCRIPTABLEBASE_H__

#include <string>
#include "npfunctions.h"
#include "UPlugin/npUPlugin.h"

namespace UPlugin {

	class UPlugin_API ScriptablePluginObjectBase : public NPObject
	{
	protected:
		NPP mNpp;
		NPWindow *m_pWindow;
		bool m_has_window;
		bool m_use_dotnetsupport;
		std::string m_identifier;

		virtual bool setWindow(NPWindow *pWindow);

	public:
		const bool hasWindow() const {
			return m_has_window;	
		};
		const bool useDotNetSupport() const {
			return m_use_dotnetsupport;
		};

		virtual void initDotNetSupport(const std::string &app_base_dir) {
		};

		virtual const std::string &getIdentifier() {
			return m_identifier;
		}

		// Called from onNPP_Destroy
		virtual void shutdown() = 0;

	public:
		ScriptablePluginObjectBase(NPP npp);
		virtual ~ScriptablePluginObjectBase();
		virtual bool setWindowBase(NPWindow *pWindow);

		// Virtual NPObject hooks called through this base class. Override
		// as you see fit.
		virtual void Invalidate();
		virtual bool HasMethod(NPIdentifier name);
		virtual bool Invoke(NPIdentifier name, const NPVariant *args,
			uint32_t argCount, NPVariant *result);
		virtual bool InvokeDefault(const NPVariant *args, uint32_t argCount,
			NPVariant *result);
		virtual bool HasProperty(NPIdentifier name);
		virtual bool GetProperty(NPIdentifier name, NPVariant *result);
		virtual bool SetProperty(NPIdentifier name, const NPVariant *value);
		virtual bool RemoveProperty(NPIdentifier name);
		virtual bool Enumerate(NPIdentifier **identifier, uint32_t *count);
		virtual bool Construct(const NPVariant *args, uint32_t argCount,
			NPVariant *result);

	public:
		static void _Deallocate(NPObject *npobj);
		static void _Invalidate(NPObject *npobj);
		static bool _HasMethod(NPObject *npobj, NPIdentifier name);
		static bool _Invoke(NPObject *npobj, NPIdentifier name,
			const NPVariant *args, uint32_t argCount,
			NPVariant *result);
		static bool _InvokeDefault(NPObject *npobj, const NPVariant *args,
			uint32_t argCount, NPVariant *result);
		static bool _HasProperty(NPObject * npobj, NPIdentifier name);
		static bool _GetProperty(NPObject *npobj, NPIdentifier name,
			NPVariant *result);
		static bool _SetProperty(NPObject *npobj, NPIdentifier name,
			const NPVariant *value);
		static bool _RemoveProperty(NPObject *npobj, NPIdentifier name);
		static bool _Enumerate(NPObject *npobj, NPIdentifier **identifier,
			uint32_t *count);
		static bool _Construct(NPObject *npobj, const NPVariant *args,
			uint32_t argCount, NPVariant *result);

	};

	typedef unsigned long long Managed_ScriptableBase_PTR_TYPE;

#define DECLARE_NPOBJECT_CLASS_WITH_BASE(_class, ctor)                        \
	static NPClass s##_class##_NPClass = {                                        \
	NP_CLASS_STRUCT_VERSION_CTOR,                                               \
	ctor,                                                                       \
	UPlugin::ScriptablePluginObjectBase::_Deallocate,                           \
	UPlugin::ScriptablePluginObjectBase::_Invalidate,                           \
	UPlugin::ScriptablePluginObjectBase::_HasMethod,                            \
	UPlugin::ScriptablePluginObjectBase::_Invoke,                               \
	UPlugin::ScriptablePluginObjectBase::_InvokeDefault,                        \
	UPlugin::ScriptablePluginObjectBase::_HasProperty,                          \
	UPlugin::ScriptablePluginObjectBase::_GetProperty,                          \
	UPlugin::ScriptablePluginObjectBase::_SetProperty,                          \
	UPlugin::ScriptablePluginObjectBase::_RemoveProperty,                       \
	UPlugin::ScriptablePluginObjectBase::_Enumerate,                            \
	UPlugin::ScriptablePluginObjectBase::_Construct                             \
	}

#define GET_NPOBJECT_CLASS(_class) &s##_class##_NPClass

};

#endif // __UPLUGIN_SCRIPTABLEBASE_H__

