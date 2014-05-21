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

#include "DebugOutput.h"
#include <stdarg.h>
#include <stdio.h>
#include <OpenSG/OSGThreadManager.h>
#include <OpenSG/OSGThread.h>

#include <sstream>

#ifdef USE_PTHREADS
	#include <pthread.h>
	#include <execinfo.h>
#endif

#if OSG_MAJOR_VERSION >= 2
static OSG::LockRefPtr printdLock = NULL;
#else //OpenSG1:
static OSG::Lock* printdLock = NULL;
#endif
static FILE* _printd_stream = stderr;
static SEVERITY _printd_severity = INFO;

void printd_severity(SEVERITY severity) {
	_printd_severity = severity;
} // printd_severity

void printd_init(FILE* stream) {
	_printd_stream = stream;
} // printd_init

void printd_finalize() {
	fflush(_printd_stream);
} // printd_finalize

INVRS_SYSTEMCORE_API void printd(const char* fmt, ...) {
#ifdef DEBUGOUTP
	if (!printdLock)
#if OSG_MAJOR_VERSION >= 2
		printdLock = OSG::dynamic_pointer_cast<OSG::Lock> (OSG::ThreadManager::the()->getLock("printdLock",false));
#else //OpenSG1:
		printdLock = dynamic_cast<OSG::Lock*> (OSG::ThreadManager::the()->getLock("printdLock"));
#endif

	if (printdLock)
#if OSG_MAJOR_VERSION >= 2
		printdLock->acquire();
#else //OpenSG1:
		printdLock->aquire();
#endif

	va_list args;
	va_start(args, fmt);

	fprintf(_printd_stream, "(\?\?) ");
#ifdef USE_PTHREADS
	fprintf(_printd_stream, "Thread 0x%08X ", (unsigned int)pthread_self());
#endif

	vfprintf(_printd_stream, fmt, args);

	va_end(args);

	if (printdLock)
		printdLock->release();
#endif
}


INVRS_SYSTEMCORE_API void _printd(SEVERITY severity, const char* fun, unsigned int line, const char* fmt, ...) {
#ifdef DEBUGOUTP
	if ((int) severity < _printd_severity)
		return;
		
	std::ostringstream s;
	s << fun << ":" << line << ": " << fmt;
	
	va_list args;
	va_start(args, fmt);
	
	vprintd(severity, s.str().c_str(), args);
	
	va_end(args);
#endif
}

INVRS_SYSTEMCORE_API void vprintd(SEVERITY severity, const char* fmt, va_list args) {
#ifdef DEBUGOUTP
	if ((int) severity < _printd_severity)
		return;
	
	if (!printdLock)
#if OSG_MAJOR_VERSION >= 2
		printdLock = OSG::dynamic_pointer_cast<OSG::Lock> (OSG::ThreadManager::the()->getLock("printdLock",false));
#else //OpenSG1:
		printdLock = dynamic_cast<OSG::Lock*> (OSG::ThreadManager::the()->getLock("printdLock"));
#endif

	if (printdLock)
#if OSG_MAJOR_VERSION >= 2
		printdLock->acquire();
#else //OpenSG1:
		printdLock->aquire();
#endif

	switch(severity) {
	case DEBUG:
		fprintf(_printd_stream, "(DD) ");
		break;
	case INFO:
		fprintf(_printd_stream, "(II) ");
		break;
	case WARNING:
		fprintf(_printd_stream, "(WW) ");
		break;
	case ERROR:
		fprintf(_printd_stream, "(EE) ");
		break;
	default:
		fprintf(_printd_stream, "(\?\?) ");
	}

#ifdef USE_PTHREADS
	fprintf(_printd_stream, "Thread 0x%08X ", (unsigned int)pthread_self());
#endif

	vfprintf(_printd_stream, fmt, args);

	if (_printd_stream != stderr && _printd_stream != stdout && severity
			== ERROR)
		fflush(_printd_stream);

	if (printdLock)
		printdLock->release();
#endif
}


INVRS_SYSTEMCORE_API void printd(SEVERITY severity, const char* fmt, ...) {
#ifdef DEBUGOUTP
	if ((int) severity < _printd_severity)
		return;
		
	va_list args;
	va_start(args, fmt);

	vprintd(severity, fmt, args);

	va_end(args);
#endif
} // printd

INVRS_SYSTEMCORE_API void printStacktrace() {
#if defined(DEBUGOUTP)
	if (!printdLock)
#if OSG_MAJOR_VERSION >= 2
		printdLock = OSG::dynamic_pointer_cast<OSG::Lock> (OSG::ThreadManager::the()->getLock("printdLock",false));
#else //OpenSG1:
		printdLock = dynamic_cast<OSG::Lock*> (OSG::ThreadManager::the()->getLock("printdLock"));
#endif

	if (printdLock)
#if OSG_MAJOR_VERSION >= 2
		printdLock->acquire();
#else //OpenSG1:
		printdLock->aquire();
#endif

#if defined(USE_PTHREADS)
	fprintf(_printd_stream, "(DD) BEGIN Stack information\n");
	// backtrace_symbols_fd can "overtake" fprintf:
	fflush(_printd_stream);
	void *array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);

	// print out all the frames to stderr
	backtrace_symbols_fd(array, size, fileno(_printd_stream));
	fprintf(_printd_stream, "(DD) END Stack information\n");
#else
	fprintf(_printd_stream, "(DD) No backtrace available.\n");
#endif

	fflush(_printd_stream);
	if (printdLock)
		printdLock->release();
#endif
} //printStacktrace
