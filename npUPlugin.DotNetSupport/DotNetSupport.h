// npUPlugin.DotNetSupport.h

#pragma once

#include "UPlugin/ScriptableBase.h"
#include "DotNetInstance.h"

using namespace System;
using namespace System::Threading;
using namespace System::Collections::Generic;

namespace UPlugin {
namespace DotNetSupport {

ref class DotNetInstanceManager
{
private:
	static DotNetInstanceManager^ singleton = nullptr;

	typedef unsigned long long NATIVE_PTR_TYPE;
	Dictionary<NATIVE_PTR_TYPE, DotNetInstance^>^ plugin_to_instance;
	Dictionary<DotNetInstance^, NATIVE_PTR_TYPE>^ instance_to_plugin;

public:
	DotNetInstanceManager() {
		plugin_to_instance = gcnew Dictionary<NATIVE_PTR_TYPE, DotNetInstance^>();
		instance_to_plugin = gcnew Dictionary<DotNetInstance^, NATIVE_PTR_TYPE>();
	}

	void Add(ScriptablePluginObjectBase *p, DotNetInstance^ vi) {
		plugin_to_instance->Add((NATIVE_PTR_TYPE)p, vi);
		Monitor::Enter(instance_to_plugin);
		try {
			instance_to_plugin->Add(vi, (NATIVE_PTR_TYPE)p);
			Monitor::PulseAll(instance_to_plugin);
		} finally {
			Monitor::Exit(instance_to_plugin);
		}
	}

	void Remove(ScriptablePluginObjectBase *p) {
		if (plugin_to_instance->ContainsKey((NATIVE_PTR_TYPE)p)) {
			DotNetInstance^ vi = plugin_to_instance[(NATIVE_PTR_TYPE)p];
			plugin_to_instance->Remove((NATIVE_PTR_TYPE)p);
			Monitor::Enter(instance_to_plugin);
			try {
				instance_to_plugin->Remove(vi);
				Monitor::PulseAll(instance_to_plugin);
			} finally {
				Monitor::Exit(instance_to_plugin);
			}
		}
	}

	static DotNetInstanceManager ^Instance() {
		if (singleton == nullptr) {
			singleton = gcnew DotNetInstanceManager();
		}
		return singleton;
	}

	ScriptablePluginObjectBase *getScriptable(DotNetInstance ^vi) {
		if (instance_to_plugin->ContainsKey(vi)) {
			return (ScriptablePluginObjectBase *)instance_to_plugin[vi];
		}
		return 0; // not found
	}

	DotNetInstance ^getDotNetInstance(NATIVE_PTR_TYPE p) {
		if (plugin_to_instance->ContainsKey((NATIVE_PTR_TYPE)p)) {
			return plugin_to_instance[(NATIVE_PTR_TYPE)p];
		}
		return nullptr; // not found
	}

	DotNetInstance ^getDotNetInstance(ScriptablePluginObjectBase *p) {
		return this->getDotNetInstance((NATIVE_PTR_TYPE)p);
	}
};

inline void RegisterDotNetObject(ScriptablePluginObjectBase *p, DotNetInstance ^instance) {
	DotNetInstanceManager::Instance()->Add(p, instance);
}

inline DotNetInstance ^GetDotNetObject(ScriptablePluginObjectBase *p) {
	return DotNetInstanceManager::Instance()->getDotNetInstance(p);
}

inline ScriptablePluginObjectBase *GetNPScriptable(DotNetInstance ^vi) {
	return DotNetInstanceManager::Instance()->getScriptable(vi);
}


}
}
