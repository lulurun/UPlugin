#ifndef __NPUPLUGIN_H__
#define __NPUPLUGIN_H__

#if defined(_WIN32)
	#if defined(UPLUGIN_EXPORTS)
		#define UPlugin_API __declspec(dllexport)
	#else
		#define UPlugin_API __declspec(dllimport)	
	#endif
#endif

#endif // __NPUPLUGIN_H__

