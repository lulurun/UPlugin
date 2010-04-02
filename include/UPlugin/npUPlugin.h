#ifndef __UPLUGIN_NPUPLUGIN_H__
#define __UPLUGIN_NPUPLUGIN_H__

#if defined(_WIN32)
	#if defined(UPLUGIN_EXPORTS)
		#define UPlugin_API __declspec(dllexport)
	#else
		#define UPlugin_API __declspec(dllimport)	
	#endif
#endif

#endif // __UPLUGIN_NPUPLUGIN_H__

