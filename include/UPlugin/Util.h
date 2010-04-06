#ifndef __UPlugin_Util_H__
#define __UPlugin_Util_H__

#include <string>
#include "npapi.h"
#include "npruntime.h"
#include "UPlugin/npUPlugin.h"

namespace UPlugin {

class UPlugin_API Util
{
public:
	static NPIdentifier GetStringIdentifier(const NPUTF8 *name);

	static void *MemAlloc(size_t n);

	static NPObject *CreateNPObject(NPP npp, NPClass *npclass);

	static std::string NPVariantToString(NPVariant value);

	static bool NPVariantParseInt(NPVariant value, int* result);

	static bool NPVariantParseDouble(NPVariant value, double* result);

};

};

#endif // __UPlugin_Util_H__

