#include <sstream>
#include "UPlugin/Util.h"

using namespace UPlugin;

NPIdentifier Util::GetStringIdentifier(const NPUTF8 *name)
{
	return NPN_GetStringIdentifier(name);
}


void *Util::MemAlloc(size_t n)
{
	return NPN_MemAlloc(n);
}

NPObject *Util::CreateNPObject(NPP npp, NPClass *npclass)
{
	return NPN_CreateObject(npp, npclass);
};

std::string Util::NPVariantToString(NPVariant value)
{
	std::stringstream ss;
	if (NPVARIANT_IS_NULL(value)) {
		return "";
	} else if (NPVARIANT_IS_INT32(value)) {
		ss << NPVARIANT_TO_INT32(value);
		return ss.str();
	} else if (NPVARIANT_IS_DOUBLE(value)) {
		ss << NPVARIANT_TO_DOUBLE(value);
		return ss.str();
	} else if (NPVARIANT_IS_STRING(value)) {
		NPString ns = NPVARIANT_TO_STRING(value);
		std::string str;
		str.assign(ns.UTF8Characters, ns.UTF8Length);
		return str;
	} else {
		return "";
	}
}

bool Util::NPVariantParseInt(NPVariant value, int* result)
{
	if (NPVARIANT_IS_NULL(value)) {
		*result = 0;
		return true;
	} else if (NPVARIANT_IS_INT32(value)) {
		*result = NPVARIANT_TO_INT32(value);
		return true;
	} else if (NPVARIANT_IS_DOUBLE(value)) {
		*result = (int32_t)NPVARIANT_TO_DOUBLE(value);
		return true;
	} else if (NPVARIANT_IS_STRING(value)) {
		return false;
	} else { 
		return false;
	}
}

bool Util::NPVariantParseDouble(NPVariant value, double* result)
{
	if (NPVARIANT_IS_NULL(value)) {
		*result = 0;
		return true;
	} else if (NPVARIANT_IS_INT32(value)) {
		*result = (double)NPVARIANT_TO_INT32(value);
		return true;
	} else if (NPVARIANT_IS_DOUBLE(value)) {
		*result = NPVARIANT_TO_DOUBLE(value);
		return true;
	} else if (NPVARIANT_IS_STRING(value)) {
		return false;
	} else { 
		return false;
	}
}
