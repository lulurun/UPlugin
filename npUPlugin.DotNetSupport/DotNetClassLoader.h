#pragma once

namespace UPlugin {
namespace DotNet {

using namespace System;
using namespace System::Collections::Generic;
using namespace System::IO;
using namespace System::Reflection;

public ref class ClassLoader
{
private:
	System::Reflection::Assembly ^OnAssemblyResolve(System::Object ^sender, System::ResolveEventArgs ^args) {
		String ^assemblyName = args->Name;
		if (assemblyName->IndexOf(",") != -1)
			assemblyName = args->Name->Substring(0, args->Name->IndexOf(","));

		for each (String ^s in m_lib_path_list)
		{
			String ^path = Path::Combine(s, assemblyName) + ".dll";

			if (File::Exists(path))
				return Assembly::LoadFrom(path);
		}

		return nullptr;
	}

	List<String ^> ^m_lib_path_list;
	String ^m_dll_path;
	String ^m_domain_name;
	System::AppDomain ^m_local_appdomain;

	Assembly ^m_assm;

	bool m_use_strict; // TODO @@@ check the dll version carefully

//	friend class ClassLoaderManager;
//private:
public:
	ClassLoader(String ^dll_path) {
		m_lib_path_list = gcnew List<String ^>();
		m_lib_path_list->Add("");

		m_dll_path = dll_path;

		System::Guid ^guid = System::Guid::NewGuid();
		m_domain_name = guid->ToString();

		System::AppDomainSetup ^ads = gcnew System::AppDomainSetup();
		ads->ApplicationName = m_domain_name;
		ads->ApplicationBase = System::AppDomain::CurrentDomain->BaseDirectory;

		m_local_appdomain = System::AppDomain::CreateDomain(m_domain_name, nullptr, ads);

		System::AppDomain::CurrentDomain->AssemblyResolve += gcnew ResolveEventHandler(this, &ClassLoader::OnAssemblyResolve);

		m_assm = nullptr;
	}

	void unload() {
		if (m_local_appdomain != nullptr)
		{
			AppDomain::Unload(m_local_appdomain);
			m_local_appdomain = nullptr;
		}
	}

public:
	property String ^DLLPath {
		String ^get() { return m_dll_path; }
	}

	void addLibPath(String ^lib_path) {
		// TODO @@@ important !! lock
		if (!this->m_lib_path_list->Contains(lib_path)) {
			this->m_lib_path_list->Add(lib_path);
		}
	}

	System::Object ^createInstance(System::String ^class_name) {
		if (m_assm == nullptr) {
			m_assm = m_local_appdomain->Load(AssemblyName::GetAssemblyName(m_dll_path));
		}
		// @@@ m_assm might be nullptr
		if (m_assm != nullptr) {
			return m_assm->CreateInstance(class_name);
		}
		return nullptr;
	}

};

public ref class ClassLoaderManager {
private:

	ref class RefCountObject {
	public:
		ClassLoader ^loader;
		int ref_count;
		RefCountObject(ClassLoader ^_loader) {
			this->loader = _loader;
			this->ref_count = 1;
		};
	};

	static ClassLoaderManager ^instance = nullptr;
	Dictionary<String ^, RefCountObject^> ^m_loader_list;

	ClassLoaderManager() {
		m_loader_list = gcnew Dictionary<String ^, RefCountObject^>();
	}

public:
	static ClassLoaderManager ^Instance() {
		if (instance == nullptr) {
			instance = gcnew ClassLoaderManager();
		}
		return instance;
	}

	ClassLoader ^GetOrCreateClassLoader(String ^dll_path) {
		// TODO @@@ important !! lock 
		if (m_loader_list->ContainsKey(dll_path)) {
			// Get
			m_loader_list[dll_path]->ref_count++;
			return m_loader_list[dll_path]->loader;
		} else {
			// Create
			ClassLoader ^loader = gcnew ClassLoader(dll_path);
			RefCountObject ^refobj = gcnew RefCountObject(loader);
			m_loader_list->Add(dll_path, refobj);
			return loader;
		}
	}

	void RemoveClassLoader(String ^dll_path) {
		// TODO @@@ important !! lock 
		if (m_loader_list->ContainsKey(dll_path)) {
			m_loader_list[dll_path]->ref_count--;
			if (m_loader_list[dll_path]->ref_count == 0) {
				m_loader_list[dll_path]->loader->unload();
				m_loader_list->Remove(dll_path);
			}
		}
	}

	void RemoveClassLoader(ClassLoader ^loader) {
		RemoveClassLoader(loader->DLLPath);
	}
};

}
}

