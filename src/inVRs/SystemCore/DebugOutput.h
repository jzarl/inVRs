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

#ifndef _DEBUGOUTP_H
#define _DEBUGOUTP_H

#include "Platform.h"
#ifndef WIN32
#include "/usr/include/stdio.h"
#else
#include <stdio.h>
#endif

#define DEBUGOUTP
#if !defined WIN32 && !defined BIGENDIAN
#define USE_PTHREADS
#endif

#ifdef ERROR
#undef ERROR
#endif

enum SEVERITY {
	DEBUG, INFO, WARNING, ERROR, UNKNOWN
};

#define PRINTD(STATUS, FMT, ...) _printd(STATUS, __FUNCTION__ ,__LINE__,FMT, ##__VA_ARGS__)
#define PRINTD_I(FMT, ...) _printd(INFO,    __FUNCTION__ ,__LINE__,FMT, ##__VA_ARGS__)
#define PRINTD_W(FMT, ...) _printd(WARNING, __FUNCTION__ ,__LINE__,FMT, ##__VA_ARGS__)
#define PRINTD_E(FMT, ...) _printd(ERROR,   __FUNCTION__ ,__LINE__,FMT, ##__VA_ARGS__)
#define PRINTD_D(FMT, ...) _printd(DEBUG,   __FUNCTION__ ,__LINE__,FMT, ##__VA_ARGS__)


INVRS_SYSTEMCORE_API void printd_severity(SEVERITY severity);
void printd_init(FILE* stream);
void printd_finalize();

INVRS_SYSTEMCORE_API void printd(const char* fmt, ...);
INVRS_SYSTEMCORE_API void _printd(SEVERITY severity, const char* fun, unsigned int line, const char* fmt, ...);
INVRS_SYSTEMCORE_API void vprintd(SEVERITY severity, const char* fmt, va_list args);
INVRS_SYSTEMCORE_API void printd(SEVERITY severity, const char* fmt, ...);
INVRS_SYSTEMCORE_API void printStacktrace();

#endif
