#ifndef __UPlugin_ScriptableObject_H__
#define __UPlugin_ScriptableObject_H__

#include "UPlugin/npUPlugin.h"
#include "UPlugin/ScriptableBase.h"
#include "UPlugin/FuncMap.h"
#include "UPlugin/Util.h"

namespace UPlugin {

// MACROS
#define DECLARE_UPLUGIN_APP_CLASS(__class_name__)				\
static NPObject *												\
Allocate##__class_name__##Object(NPP npp, NPClass *aClass)		\
{																\
  return new __class_name__(npp);								\
}																\
																\
DECLARE_NPOBJECT_CLASS_WITH_BASE(__class_name__,				\
	Allocate##__class_name__##Object);							\
																\
extern "C" EXPORT_FUNC UPlugin::ScriptablePluginObjectBase *	\
CreateInstance_##__class_name__##(NPP npp) {					\
	return (UPlugin::ScriptablePluginObjectBase *)				\
		UPlugin::Util::CreateNPObject(npp,						\
			GET_NPOBJECT_CLASS(__class_name__));				\
}																\


template<class T>
class ScriptablePluginObject : public ScriptablePluginObjectBase
{
private:
	// TODO @@@ lock method_table
	UPluginMethodMap<T> m_method_table;

protected:
	std::string m_identifier;

public:
	ScriptablePluginObject(NPP npp = NULL)
		:ScriptablePluginObjectBase(npp)
	{
	};
	virtual ~ScriptablePluginObject()
	{
	};

	void register_method(const std::string &name, T *obj, bool (T::*method)(const NPVariant *, uint32_t, NPVariant *)){
		m_method_table.set(Util::GetStringIdentifier(name.c_str()), obj, method);
	};

	bool HasMethod(NPIdentifier name) {
		return m_method_table.contains(name);
	};

	bool Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result) {
		return this->m_method_table.invokeMethod(name, args, argCount, result);
	};

public:
	const std::string getIdentifier() const {
		return m_identifier;
	};
	const std::string getInstallDir() const {
		// TODO @@@ get from Env
		return "";
	};


};

};


#endif // __UPlugin_ScriptableObject_H__

