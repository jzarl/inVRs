#ifndef COREPLUG_FACTORYPLUGIN_H
#define COREPLUG_FACTORYPLUGIN_H

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
#include "Dso.h"

/** Template class to wrap plugin loading stuff in a nice interface.
 * A FactoryPlugin is intended for classes that provide a factory function
 * for creating instances.
 *
 *
 * Template parameters:
 * --------------------
 *
 *  - FactoryT:
 *    Type returned by the factory function (=root of type hierarchy).
 *  - TypeSource:
 *    Must provide a std::string TypeSource::basetypename which is the name of the root
 *    class of the FactoryT type hierarchy.
 */
class BASELIB_COREPLUG_API FactoryPlugin
{
	public:
		explicit FactoryPlugin( const std::string &filename, const std::string& base_type_name);
		~FactoryPlugin();

		/**
		 * Returns the (type) for the plugin.
		 * You are free to use any name you want, but using the name of the 
		 * actual class returned by the <code>FactoryT</code> is recommended.
		 *
		 * Interna
		 * -------
		 *
		 * This method returns the value of the <code>const char* CP_NAME_#TYPE#</code> symbol in the DSO,
		 * where <code>#TYPE#</code> is value of the template parameter of the same name.
		 *
		 */
		std::string name() const;

		/**
		 * Ask the plugin whether it considers itself compatible with the host system.
		 * @param versionid a freeform representation of the plugin interface version
		 *
		 * Interna
		 * -------
		 * Calls the function <code>CP_IS_COMPATIBLE</code> with versionid as parameter.
		 *
		 * A note regarding <code>versionid</code>: This represents the interface version
		 * of the class returned by <code>FactoryT</code> <em>and</em> <code>FactoryT</code> itself.
		 * If you are diligent, you may increment this manually. A better practice is to let the 
		 * build system create a hash of the interface definition files and use that instead.
		 */
		bool isCompatible( const std::string &versionid );

		/**
		 * Return the factory function for the plugin class.
		 * Always check isCompatible() before calling this method!
		 *
		 * @return a factory
		 * @throws std::runtime_error on failure
		 * @see isCompatible
		 *
		 * Interna
		 * -------
		 * Calls <code>CP_FACTORY</code> and returns its value
		 */
		const Dso& get_dso() const;

	private:
		Dso dso_;
		std::string base_type_name_;
		// non-copyable
		FactoryPlugin(const FactoryPlugin&);
		FactoryPlugin& operator=(const FactoryPlugin&);
};

template<typename FactoryT>
FactoryT create_factory(const Dso& dso)
{
	typedef FactoryT (*factoryFunction_t)();
	factoryFunction_t factoryFun = reinterpret_cast<factoryFunction_t>(dso.rawSymbol("CP_FACTORY"));
	return factoryFun();
}

#endif
