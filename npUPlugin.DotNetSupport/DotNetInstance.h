#pragma once

#include "DotNetClassLoader.h"
#include "DotNetEnv.h"

namespace UPlugin {
namespace DotNetSupport {

ref class DotNetInstance
{
protected:
	DotNetClassLoader ^m_class_loader;
	Object ^m_instance;

public:
	DotNetInstance(Object ^instance){
		m_class_loader = nullptr;
		m_instance = instance;
	};

	DotNetInstance(System::String ^class_name, System::String ^dll_name){
		m_class_loader = gcnew DotNetClassLoader(DotNetEnv::DIR_Installation);
		m_instance = m_class_loader->createInstance(
			System::IO::Path::Combine(DotNetEnv::DIR_Installation, dll_name),
			class_name);
	};
};

}
}

