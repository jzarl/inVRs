/*
 * AvataraWrapperExports.h
 *
 *  Created on: 05.12.2010
 *      Author: sam
 */

#ifndef AVATARAWRAPPEREXPORTS_H_
#define AVATARAWRAPPEREXPORTS_H_

#ifdef WIN32
	#ifdef INVRSAVATARAWRAPPER_EXPORTS
	#define INVRS_AVATARAWRAPPER_API __declspec(dllexport)
	#else
	#define INVRS_AVATARAWRAPPER_API __declspec(dllimport)
	#endif
#else
	#define INVRS_AVATARAWRAPPER_API
#endif


#endif /* AVATARAWRAPPEREXPORTS_H_ */
