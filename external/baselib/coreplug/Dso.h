#ifndef COREPLUG_DSO_H
#define COREPLUG_DSO_H

/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2013      by the Johannes Kepler University, Linz        *
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

#include <coreplug_export.h>
#include <string>

#ifdef WIN32
	#include <windows.h>
	typedef FARPROC dsosymbol_t;
	typedef HINSTANCE dsohandle_t;;
#else
	typedef void* dsosymbol_t;
	typedef void* dsohandle_t;;
#endif


/**
 * A wrapper class for the system specific DSO handling functions.
 *
 * Typical usage:
 * <code>
 * Dso myDso;
 * if ( myDso.open("/path/to/my.dso") )
 * {
 *   typedef void (*myFunctionType)();
 *   myFunctionType myFun = myDso.rawSymbol("myFun");
 *   myFun();
 * } else {
 *   std::cout << "Could not open DSO. Error message: " << myDso.lastError();
 * }
 * // myDso is automatically closed when it goes out of scope
 * </code>
 */
class BASELIB_COREPLUG_API Dso
{
	public:
		explicit Dso();
		/**
		 * Close/release the DSO handle if appropriate.
		 * Note that on Windows and Linux platforms the DSO handle is actually
		 * reference counted. A DSO is only unloaded when the last remaining DSO 
		 * handle is closed.
		 */
		~Dso();

		/**
		 * Open a DSO file.
		 *
		 * @param path File name of the DSO file (usually ends in ".so" on Linux, or ".dll" on Windows)
		 * @return <code>true</code> if the file could be loaded, <code>false</code> if an error occurred.
		 */
		bool open(const std::string &path);

		/**
		 * Check if the Dso object is ready to be used.
		 * A Dso is valid, if it has been opened without errors, i.e.
		 * if it is in a state in which the rawSymbol method may be called.
		 */
		bool isValid() const;

		/**
		 * Get the error string representing the last error that occurred.
		 * The error string is reset as soon as a non-const method (open, rawSymbol)
		 * is called. Calling the lastError method does <em>not</em> reset the message.
		 *
		 * @return a string representation of the error, or an empty string if no error is currently set.
		 */
		std::string lastError() const;

		/**
		 * Look up a symbol name in the DSO and returns it address.
		 * If a symbol exists more than once, only one address is returned (usually that of the first symbol).
		 *
		 * Note that on some platforms it is possible to have null-symbols.
		 * While the usage of such null symbols in portable code is discouraged, you can
		 * distinguish a null symbol from non-existing symbol by checking lastError().
		 *
		 * @param symbol the (mangled) name of the symbol
		 * @return The address of the symbol, of 0 if the lookup failed.
		 */
		dsosymbol_t rawSymbol(const std::string &symbol) const;

		/**
		 * Get the raw DSO handle.
		 * @return a platform specific DSO handle, or 0 if the Dso is not valid.
		 */
		dsohandle_t rawHandle() const;

	private:
		class DsoPrivate;
		DsoPrivate *state;
		// uncopyable
		Dso(const Dso&);
		Dso& operator=(const Dso&);
};

BASELIB_COREPLUG_API const char* platformDsoPrefix();
BASELIB_COREPLUG_API const char* platformDsoSuffix();

#endif
