
#ifndef COLLISIONMAPEXPORTS_H_
#define COLLISIONMAPEXPORTS_H_

#ifdef WIN32
	#ifdef INVRSCOLLISIONMAP_EXPORTS
	#define INVRS_COLLISIONMAP_API __declspec(dllexport)
	#else
	#define INVRS_COLLISIONMAP_API __declspec(dllimport)
	#endif
#else
	#define INVRS_COLLISIONMAP_API
#endif


#endif
