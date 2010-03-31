#pragma once

#include "npapi.h"
#include "npruntime.h"

using namespace System;
using namespace System::Text;

namespace UPlugin {
	namespace DotNetSupport {

		ref class DotNetUtil
		{
		public:
			static String^ NPVariantToString(NPVariant value)
			{
				if (NPVARIANT_IS_NULL(value)) {
					return String::Empty;
				} else if (NPVARIANT_IS_INT32(value)) {
					return String::Format("{0}", NPVARIANT_TO_INT32(value));
				} else if (NPVARIANT_IS_DOUBLE(value)) {
					return String::Format("{0}", NPVARIANT_TO_DOUBLE(value));
				} else if (NPVARIANT_IS_STRING(value)) {
					NPString ns = NPVARIANT_TO_STRING(value);
					return gcnew String(ns.UTF8Characters,0,ns.UTF8Length,Encoding::UTF8);
				} else {
					return nullptr;
				}
			}

			static bool NPVariantParseInt(NPVariant value, int* result)
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
					NPString ns = NPVARIANT_TO_STRING(value);
					String^ s = gcnew String(ns.UTF8Characters,0,ns.UTF8Length,Encoding::UTF8);
					try {
						*result = Int32::Parse(s);
						return true;
					} catch (Exception^) {
						return false;
					}
				} else { 
					return false;
				}
			}

			static bool NPVariantParseDouble(NPVariant value, double* result)
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
					NPString ns = NPVARIANT_TO_STRING(value);
					String^ s = gcnew String(ns.UTF8Characters,0,ns.UTF8Length,Encoding::UTF8);
					try {
						*result = Double::Parse(s);
						return true;
					} catch (Exception^) {
						return false;
					}
				} else { 
					return false;
				}
			}

		};
	}
}
