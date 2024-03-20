#pragma once

#ifdef CRN_PLATFORM_WINDOW
	#ifdef CRN_BUILD_DLL
		#define CHIRON_API __declspec(dllexport)
	#else
		#define CHIRON_API __declspec(dllimport)
	#endif // CRN_BUILD_DLL
#else
	#error Chiron only suppports Windows right now!
#endif // CRN_PLATFORM_WINDOW
