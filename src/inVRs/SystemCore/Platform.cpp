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

#include "Platform.h"
#include <time.h>
#include <string.h>

#ifdef WIN32
#include <float.h>

INVRS_SYSTEMCORE_API unsigned sleep(unsigned seconds)
{
	Sleep(1000*seconds);
	return 0;
}

INVRS_SYSTEMCORE_API int strcasecmp(const char* s1, const char* s2)
{
	return _stricmp(s1, s2);
}

INVRS_SYSTEMCORE_API int usleep(unsigned __int64 usec)
{
	unsigned msec = (unsigned)(usec/1000);
	Sleep(msec);
	return 0;
}

INVRS_SYSTEMCORE_API int isnan(float n)
{
	return _isnan((double)n);
}

/*
INVRS_SYSTEMCORE_API int gettimeofday(timeval* tv, void* timezone_set_me_to_NULL)
{
	if(tv)
	{
		DWORD ms = GetTickCount();
		tv->tv_sec = ms/1000;
		tv->tv_usec = (ms % 1000) * 1000;
	}

	return -1; // indicates failure
}
*/

#endif
