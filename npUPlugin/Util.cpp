#include "UPlugin/Util.h"

using namespace UPlugin;

NPIdentifier Util::GetStringIdentifier(const NPUTF8 *name)
{
	return NPN_GetStringIdentifier(name);
}


void *Util::MemAlloc(size_t n) {
	return NPN_MemAlloc(n);
}

NPObject *Util::CreateNPObject(NPP npp, NPClass *npclass) {
	return NPN_CreateObject(npp, npclass);
};

