#ifndef __UPLUGIN_FUNCMAP_H__
#define __UPLUGIN_FUNCMAP_H__

#include "npapi.h"
#include "npruntime.h"
#include <map>

namespace UPlugin {

template<class T>
class UPluginMethodMap {
public:
	typedef bool (T::*UPluginMethod)(const NPVariant *, uint32_t, NPVariant *);

	class Entry {
		T *_obj;
		UPluginMethod _method;

	public:

		Entry()
			:_obj(NULL), _method(NULL)
		{};
		Entry(T *obj, UPluginMethod method)
			: _obj(obj), _method(method)
		{};
		~Entry(){};

		bool invoke(const NPVariant *args, uint32_t argCount, NPVariant *result){
			return (_obj->*_method)(args, argCount, result);
		};
	};

	typedef std::map<NPIdentifier, Entry> MethodMap_TYPE;
	MethodMap_TYPE _map;

	void set(NPIdentifier, T *, UPluginMethod);
	bool contains(NPIdentifier);
	bool invokeMethod(NPIdentifier i, const NPVariant *, uint32_t, NPVariant *);
};

template<class T>
void UPluginMethodMap<T>::set(NPIdentifier i, T *obj, UPluginMethod pFunc)
{
	Entry e(obj, pFunc);
	_map[i] = e;
}

template<class T>
bool UPluginMethodMap<T>::contains(NPIdentifier i)
{
	MethodMap_TYPE::iterator it = _map.find(i);
	return it != _map.end();
}

template<class T>
bool UPluginMethodMap<T>::invokeMethod(NPIdentifier i, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	MethodMap_TYPE::iterator it = _map.find(i);
	if (it == _map.end()) {
		// TODO @@@ log not found;
		return false;
	}
	Entry &e = it->second;
	return e.invoke(args, argCount, result);
}

};

#endif // __UPLUGIN_FUNCMAP_H__
