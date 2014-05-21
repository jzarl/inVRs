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

#ifndef _COREPLUG_FACTORYPLUGINCOREAPI_H
#define _COREPLUG_FACTORYPLUGINCOREAPI_H

/* \file
 * Preprocessor macros implementing the FactoryPlugin ABI.
 *
 *
 * Macro definitions
 * -----------------
 *
 *  - <code>MAKEFACTORYPLUGIN(BASETYPENAME, TYPENAME, COMPATFUN)</code>
 *    Create the neccessary symbols for FactoryPlugin.
 *     - <code>BASETYPENAME</code>: same as TypeSource::basetypename
 *     - <code>TYPENAME</code>: type returned by FactoryT
 *     - <code>COMPATFUN</code>: a function taking a version string and returning a bool (true, if the plugin is compatible).
 *     - <code>FactoryT</code>: type of the factory function.
 */

// DSO_CALL: calling convention for DSO functions
#ifdef WIN32
#define DSO_CALL __stdcall
#else
#define DSO_CALL 
#endif

// Use DSO_PUBLIC for public linkage
#if WIN32
# define DSO_PUBLIC __declspec(dllexport)
#else
# if __GNUC__ >= 4
#  define DSO_PUBLIC __attribute__ ((visibility ("default")))
# else
#  define DSO_PUBLIC
# endif
#endif

#define MAKEFACTORYPLUGIN(BASETYPENAME, TYPENAME, COMPATFUN, FactoryT) \
	extern "C" { \
		DSO_PUBLIC const char* DSO_CALL CP_NAME_ ## BASETYPENAME \
			= # TYPENAME ; \
		DSO_PUBLIC bool DSO_CALL CP_IS_COMPATIBLE( const std::string &version) \
		{ \
			return COMPATFUN( version ); \
		} \
		DSO_PUBLIC FactoryT DSO_CALL CP_FACTORY() \
		{ \
			return TYPENAME::create; /* N.B.: we return the address of the factory function, not its result! */ \
		} \
	}

#endif

