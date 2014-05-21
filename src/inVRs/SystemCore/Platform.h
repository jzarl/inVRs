/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2005-2009 by the Johannes Kepler University, Linz        *
 *                                                                           *
 *                            www.inVRs.org                                  *
 *                                                                           *
 *              contact: canthes@inVRs.org, rlander@inVRs.org                *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _PLATFORM_H
#define _PLATFORM_H

// platform specific header files:
#ifdef WIN32
#	define WIN32_LEAN_AND_MEAN
#	define NOMINMAX
#	include <windows.h>
#else
#	include <stdint.h>
#endif

// symbol visibility in shared libraries:
#define INVRS_API extern "C"

#ifdef WIN32
#	ifdef INVRSSYSTEMCORE_EXPORTS
#		define INVRS_SYSTEMCORE_API __declspec(dllexport)
#	else
#		define INVRS_SYSTEMCORE_API __declspec(dllimport)
#	endif
#else
#	define INVRS_SYSTEMCORE_API
#endif

#ifdef WIN32
// types normally defined in stdint.h:
typedef unsigned __int64 uint64_t;
typedef unsigned int uint32_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;

INVRS_SYSTEMCORE_API unsigned sleep(unsigned seconds);
INVRS_SYSTEMCORE_API int strcasecmp(const char* s1, const char* s2);
INVRS_SYSTEMCORE_API int usleep(unsigned __int64 usec);
INVRS_SYSTEMCORE_API int isnan(float n);

// not implemented properly, better do not use it at all!
//INVRS_SYSTEMCORE_API int gettimeofday(timeval* tv, void* timezone_set_me_to_NULL);

#ifdef ERROR
#undef ERROR
#endif

#endif /* WIN32 */

#endif /* header guard */
