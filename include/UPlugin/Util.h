#ifndef __UPlugin_Util_H__
#define __UPlugin_Util_H__

#include "npapi.h"
#include "npruntime.h"
#include "npUPlugin.h"

namespace UPlugin {

class UPlugin_API Util
{
public:
	static NPIdentifier GetStringIdentifier(const NPUTF8 *name);

	static void *MemAlloc(size_t n);

	static NPObject *CreateNPObject(NPP npp, NPClass *npclass);
};

};

#endif // __UPlugin_Util_H__

