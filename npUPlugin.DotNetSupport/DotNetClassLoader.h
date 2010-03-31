#pragma once

namespace UPlugin {
namespace DotNetSupport {

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Reflection;

ref class DotNetClassLoader
{
private:
	System::AppDomain ^m_local_appdomain;
	System::String ^m_domain_name;
	System::String ^m_lib_path;

	bool m_user_strict;

public:
	property System::String ^AppDomainName { System::String ^get() { return m_domain_name; } }
	property System::String ^LibPath { System::String ^get() { return m_lib_path; } }

	DotNetClassLoader(System::String ^lib_path) {
		m_lib_path = lib_path;

		Guid ^guid = System::Guid::NewGuid();
		m_domain_name = guid->ToString();

		AppDomainSetup ^ads = gcnew AppDomainSetup();
		ads->ApplicationName = m_domain_name;
		ads->ApplicationBase = AppDomain::CurrentDomain->BaseDirectory;

		m_local_appdomain = AppDomain::CreateDomain(m_domain_name, nullptr, ads);
		AppDomain::CurrentDomain->AssemblyResolve += gcnew ResolveEventHandler(this, &DotNetClassLoader::OnAssemblyResolve);
	}

	System::Object ^createInstance(System::String ^dll_path, System::String ^class_name) {
		Assembly ^assm = m_local_appdomain->Load(AssemblyName::GetAssemblyName(dll_path));
		return assm->CreateInstance(class_name);
	}

	void Unload() {
		if (m_local_appdomain != nullptr)
		{
			AppDomain::Unload(m_local_appdomain);
			m_local_appdomain = nullptr;
		}
	}

	System::Reflection::Assembly ^OnAssemblyResolve(System::Object ^sender, System::ResolveEventArgs ^args) {
		List<String ^> ^pathList = gcnew List<String ^>();
		pathList->Add("");
		pathList->Add(m_lib_path);

		String ^assemblyName = args->Name;
		if (assemblyName->IndexOf(",") != -1)
			assemblyName = args->Name->Substring(0, args->Name->IndexOf(","));

		for each (String ^s in pathList)
		{
			String ^path = Path::Combine(Directory::GetCurrentDirectory(),
				Path::Combine(s, assemblyName)) + ".dll";

			if (File::Exists(path))
				return Assembly::LoadFrom(path);
		}

		return nullptr;
	}
};

}
}
