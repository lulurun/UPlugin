#pragma once

#include "UPlugin/ScriptableBase.h"
#include "DotNetClassLoader.h"

using namespace System::Threading;
using namespace System::Collections::Generic;

namespace UPlugin {
namespace DotNet {

// TODO @@@ Unload !!
// TODO @@@ ref counter !!
public ref class DotNetInstance
{
protected:
	Dictionary<String ^, Object ^> ^m_object_list;

public:
	DotNetInstance() {
		m_object_list = gcnew Dictionary<String ^, Object ^>();
	};

	void addObject(String ^id, Object ^obj) {
		if (!m_object_list->ContainsKey(id)) {
			m_object_list->Add(id, obj);
		}
	}

	Object ^getObject(String ^id) {
		if (m_object_list->ContainsKey(id)) {
			return m_object_list[id];
		}
		return nullptr;
	}
};

// TODO @@@ lock !!
// TODO @@@ unload & ref counter !!
public ref class DotNetInstanceManager
{
private:
	static DotNetInstanceManager^ singleton = nullptr;

	Dictionary<Managed_ScriptableBase_PTR_TYPE, DotNetInstance^>^ plugin_to_instance;
	Dictionary<DotNetInstance^, Managed_ScriptableBase_PTR_TYPE>^ instance_to_plugin;

	Object ^m_lock;

public:
	DotNetInstanceManager() {
		m_lock = gcnew Object();
		plugin_to_instance = gcnew Dictionary<Managed_ScriptableBase_PTR_TYPE, DotNetInstance^>();
		instance_to_plugin = gcnew Dictionary<DotNetInstance^, Managed_ScriptableBase_PTR_TYPE>();
	}

	bool Contains(Managed_ScriptableBase_PTR_TYPE p) {
		Monitor::Enter(m_lock);
		try {
			bool res = plugin_to_instance->ContainsKey(p);
			Monitor::PulseAll(m_lock);
			return res;
		} finally {
			Monitor::Exit(m_lock);
		}
		return false;
	}

	void Add(Managed_ScriptableBase_PTR_TYPE p, DotNetInstance^ vi) {
		Monitor::Enter(m_lock);
		try {
			plugin_to_instance->Add(p, vi);
			instance_to_plugin->Add(vi, p);
			Monitor::PulseAll(m_lock);
		} finally {
			Monitor::Exit(m_lock);
		}
	}

	void Remove(Managed_ScriptableBase_PTR_TYPE p) {
		Monitor::Enter(m_lock);
		try {
			if (plugin_to_instance->ContainsKey(p)) {
				DotNetInstance^ vi = plugin_to_instance[p];
				plugin_to_instance->Remove(p);
				instance_to_plugin->Remove(vi);
			}
			Monitor::PulseAll(m_lock);
		} finally {
			Monitor::Exit(m_lock);
		}
	}

	static DotNetInstanceManager ^Instance() {
		if (singleton == nullptr) {
			singleton = gcnew DotNetInstanceManager();
		}
		return singleton;
	}

	Managed_ScriptableBase_PTR_TYPE getScriptable(DotNetInstance ^vi) {
		Monitor::Enter(m_lock);
		try {
			if (instance_to_plugin->ContainsKey(vi)) {
				Managed_ScriptableBase_PTR_TYPE p = instance_to_plugin[vi];
				Monitor::PulseAll(m_lock);
				return p;
			}
		} finally {
			Monitor::Exit(m_lock);
		}
		return 0; // not found
	}

	DotNetInstance ^getDotNetInstance(Managed_ScriptableBase_PTR_TYPE p) {
		Monitor::Enter(m_lock);
		try {
			if (plugin_to_instance->ContainsKey(p)) {
				DotNetInstance ^vi = plugin_to_instance[p];
				Monitor::PulseAll(m_lock);
				return vi;
			}
		} finally {
			Monitor::Exit(m_lock);
		}
		return nullptr; // not found
	}

};


}
}

