// npUPlugin.DotNetSupport.h

#pragma once

#include "UPlugin/ScriptableBase.h"
#include "DotNetInstance.h"

using namespace System;

namespace UPlugin {
namespace DotNet {

// TODO @@@ support multiple instance
public ref class DotNetSupport {
public:
	static void RegisterDotNetObject(Managed_ScriptableBase_PTR_TYPE p, String ^class_name, String ^dll_name, String ^lib_path) {
		String ^dll_path = Path::Combine(lib_path, dll_name);
		ClassLoader ^loader = ClassLoaderManager::Instance()->GetOrCreateClassLoader(dll_path);
		loader->addLibPath(lib_path);
		Object ^obj = loader->createInstance(class_name);
		DotNetInstance ^instance = gcnew DotNetInstance();
		instance->addObject("fixed_name", obj);
		DotNetInstanceManager::Instance()->Add(p, instance);
	}

	static Object ^GetDotNetObject(Managed_ScriptableBase_PTR_TYPE p, String ^object_name) {
		DotNetInstance ^instance = DotNetInstanceManager::Instance()->getDotNetInstance(p);
		if (instance != nullptr) {
			return instance->getObject("fixed_name");
		}
		return nullptr;
	}

	static Managed_ScriptableBase_PTR_TYPE GetNPScriptable(DotNetInstance ^vi) {
		return DotNetInstanceManager::Instance()->getScriptable(vi);
	}

};

}
}
