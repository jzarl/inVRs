#ifndef INVRS_SYSTEMCORE_MODULECOREAPI_HEADER_GUARD_H
#define INVRS_SYSTEMCORE_MODULECOREAPI_HEADER_GUARD_H

#include "./ModuleInterface.h"

#ifdef WIN32

#define INVRS_MODULE_API __declspec(dllexport)

#define MAKEMODULEPLUGIN(Module_t, ModuleOwner_t) \
	extern "C" { \
		ModuleInterface* module_ ## Module_t = 0; \
		INVRS_MODULE_API const char* INVRS_MODULE_CALL modulePluginVersion() \
		{ \
			return MODULEPLUGINVERSION; \
		} \
		INVRS_MODULE_API bool INVRS_MODULE_CALL registerModule() \
		{ \
			if (!module_ ## Module_t) { \
				module_ ## Module_t = new Module_t(); /* new could be overloaded outside this compilation unit, so we have to use this function. */ \
				return ModuleOwner_t::registerModuleInterface( module_ ## Module_t ); \
			}\
			return true; /* already registered*/\
		} \
		INVRS_MODULE_API bool INVRS_MODULE_CALL unregisterModule() \
		{ \
			bool ret = false; \
			if (module_ ## Module_t) { \
				printd("%s::unregisterModule(): unregistering Module.\n", #Module_t); \
				ret = ModuleOwner_t::unregisterModuleInterface( module_ ## Module_t ); \
				printd("%s::unregisterModule(): deleting Module.\n", #Module_t); \
				delete module_ ## Module_t; /* delete could be overloaded outside this compilation unit, so we have to use this function. */ \
			} \
			else { \
				printd(ERROR, "%s::unregisterModule(): Module already deleted!\n", #Module_t); \
			} \
			return ret; \
		} \
	}\
	BOOL __stdcall DllMain( HMODULE hModule, \
						   DWORD  ul_reason_for_call, \
						   LPVOID lpReserved \
						 ) \
	{ \
		switch (ul_reason_for_call)\
		{ \
			case DLL_PROCESS_ATTACH: \
				fprintf(stderr, "%s::DllMain(): process attached.\n", #Module_t); \
				SystemCore::addPreloadModule(hModule);\
				break;\
			case DLL_THREAD_ATTACH: \
				fprintf(stderr, "%s::DllMain(): thread attached.\n", #Module_t); \
				break;\
			case DLL_THREAD_DETACH: \
			case DLL_PROCESS_DETACH: \
				break; \
		} \
		return TRUE; \
	}

#else

#define MAKEMODULEPLUGIN(Module_t, ModuleOwner_t) \
	extern "C" { \
		ModuleInterface* module_ ## Module_t = 0; \
		const char* modulePluginVersion() \
		{ \
			return MODULEPLUGINVERSION; \
		} \
		bool registerModule() \
		{ \
			module_ ## Module_t = new Module_t(); /* new could be overloaded outside this compilation unit, so we have to use this function. */ \
			return ModuleOwner_t::registerModuleInterface( module_ ## Module_t ); \
		} \
		bool unregisterModule() \
		{ \
			bool ret = false; \
			if (module_ ## Module_t) { \
				printd("%s::unregisterModule(): unregistering Module.\n", #Module_t); \
				ret = ModuleOwner_t::unregisterModuleInterface( module_ ## Module_t ); \
				printd("%s::unregisterModule(): deleting Module.\n", #Module_t); \
				delete module_ ## Module_t; /* delete could be overloaded outside this compilation unit, so we have to use this function. */ \
			} \
			else { \
				printd(ERROR, "%s::unregisterModule(): Module already deleted!\n", #Module_t); \
			} \
			return ret; \
		} \
	}
#endif

// export non-mangled names of core api
#ifdef _MSC_VER
	// pragma replaces .def file
	#pragma comment(linker,"/EXPORT:modulePluginVersion=_modulePluginVersion@0")
	#pragma comment(linker,"/EXPORT:registerModule=_registerModule@0")
	#pragma comment(linker,"/EXPORT:unregisterModule=_unregisterModule@0")
#endif

#endif
