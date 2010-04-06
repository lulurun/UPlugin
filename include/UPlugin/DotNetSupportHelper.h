#ifdef UPLUGIN_DOTNET

#include "UPlugin/Env.h"
#include "UPlugin/ScriptableBase.h"

namespace UPlugin {

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Reflection;

public ref class DotNetSupportHelper
{
private:
	List<String ^> ^m_path_list;
	Object ^m_support;

	static DotNetSupportHelper ^_instance = gcnew DotNetSupportHelper();
	// TODO @@@ unload dotnotsupport.dll
	DotNetSupportHelper() {
		const std::string &dll_path = Env::GetInstance()->getDotNetSupportDLL();
		String ^dll_path_managed = gcnew String(dll_path.c_str());
		m_path_list = gcnew List<String ^>();
		m_path_list->Add(Path::GetDirectoryName(dll_path_managed));
		AppDomain::CurrentDomain->AssemblyResolve += gcnew ResolveEventHandler(this, &DotNetSupportHelper::OnAssemblyResolve);
		// load dotnetsupport
		Assembly ^assm = AppDomain::CurrentDomain->Load(AssemblyName::GetAssemblyName(dll_path_managed));
		m_support = assm->CreateInstance("UPlugin.DotNet.DotNetSupport");
		if (m_support == nullptr) {
			// TODO @@@ log
		}
	};

	System::Reflection::Assembly ^OnAssemblyResolve(System::Object ^sender, System::ResolveEventArgs ^args) {
		String ^assemblyName = args->Name;
		if (assemblyName->IndexOf(",") != -1)
			assemblyName = args->Name->Substring(0, args->Name->IndexOf(","));

		for each (String ^s in m_path_list)
		{
			String ^path = Path::Combine(Directory::GetCurrentDirectory(),
				Path::Combine(s, assemblyName)) + ".dll";

			if (File::Exists(path))
				return Assembly::LoadFrom(path);
		}
		return nullptr;
	}

public:
	static DotNetSupportHelper ^GetInstance() {
		return _instance;
	}

	void AddLoadLibraryPath(System::String ^path)
	{
		if (!m_path_list->Contains(path)) {
			m_path_list->Add(path);
		}
	};

	void RegisterDotNetObject(Managed_ScriptableBase_PTR_TYPE p, String ^class_name, String ^dll_name, String ^lib_path)
	{
		if (m_support != nullptr) {
			UPlugin::DotNet::DotNetSupport ^support = (UPlugin::DotNet::DotNetSupport ^)m_support;
			support->RegisterDotNetObject(p, class_name, dll_name, lib_path);
		}
	};

	Object ^GetDotNetObject(Managed_ScriptableBase_PTR_TYPE p, String ^object_name)
	{
		if (m_support != nullptr) {
			UPlugin::DotNet::DotNetSupport ^support = (UPlugin::DotNet::DotNetSupport ^)m_support;
			return support->GetDotNetObject(p, object_name);
		}
		return nullptr;
	};

};

}

#endif
