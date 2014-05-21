#ifndef _LOADER_H_
#define _LOADER_H_

/***************************************************************************
 *   Copyright (C) 2007 by Thomas Webernorfer                              *
 *   thomas_weberndorfer@hotmail.com                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "XMLNode.h"
#include "Config.h"
#include <dlfcn.h>

/**
 * \brief This class provides methods for loading an object from a library and destroying it.
 *
 * Creating objects directly out of a library is not possible. So two
 * factory-methods are used:
 *
 * - One for creating a new object, which simply calls the constructor.
 * - One for destructing an object. Each object stores a pointer to this
 *   factory-method, because only this method can destroy the object without crashes.
 *
 * This kind of object, which has been loaded from a library, has been named
 * "loaded-object".
 *
 * It is improtant to declare a virtual destructor in each class, which uses
 * this class as superclass. Without this virtual destructor, the programm
 * may crash during deletion of a loaded-object.
 *
 *
 * @date 3rd of May 2007
 * @author Thomas Weberndorfer
 *
 **/
template <typename T> class Loader
{

    private:

        // Defines the type of a method-pointer for creating an loaded-object
        typedef T* _create_w ( XMLNode* );
        // Defines the type of a method-pointer for destroying an loaded-object
        typedef void _destroy_w ( T* );
        // This method-pointer points at the destruction-factory-method
        // in the loaded library containing the object.
        _destroy_w *clean;
        // This is used to remember the pointer to the library
        // in which the object is stored.
        void *lib;

    public:

        /**
         * This method is used to destroy a loaded-object. <b>Do not delete an loaded-object
         * with <code>delete</code>!</b>
         * @param obj This pointer must point to the loaded-object, which should be deleted.
         **/
        static void deleteObject ( T *obj )
        {
            void *lib = obj->lib;
            // Deleting object
            obj->clean ( obj );
            // Unloading library
            dlclose ( lib );
        }

        /**
         * This method creates and returns a loaded-object out of a library. The library-filename and
         * the classname must be defined in the XML-Node, which is wrapped in the XMLNode -object.
         * The used library must be of type "shared object", must provide the two factory-methods
         * and of course the class.
         * @param xml Pass an XMLNode -object focusing on the correct XML-node.
         * @return This method returns an instance of a loaded-object or NULL in case of an error.
         **/
        static T* createObject ( XMLNode *xml )
        {
            // Load the library containing the object
            const char* ldpath = xml->getAttributeValue ( "library" );
            if ( ldpath == NULL )
            {
                ERR ( "Library path is missing in node <" << xml->getNodeName() << ">." );
                return NULL;
            }
            // Library attribute found - load the library
            void *lib = dlopen ( ldpath, RTLD_LAZY );
            if ( !lib )
            {
                ERR ( "Cannot load library: " << dlerror() );
                return NULL;
            }
            // Load class name
            const char * classname = xml->getAttributeValue ( "classname" );
            if ( classname == NULL )
            {
                ERR ( "The object can not be loaded with a missing classname in node <" << xml->getNodeName() << ">." );
                return NULL;
            }
            // Create symbols for the factory-methods
            std::string createHandle = "_create_";
            createHandle  += classname;
            std::string destroyHandle = "_destroy_";
            destroyHandle  += classname;
            // Reset errors
            dlerror();
            // Load symbols
            // Load create-method
            _create_w *create = ( _create_w* ) dlsym ( lib, createHandle.c_str() );
            if ( dlerror() )
            {
                ERR ( "Cannot load symbol <" << createHandle << ">." );
                return NULL;
            }
            // Load destroy-method
            _destroy_w *destroy = ( _destroy_w* ) dlsym ( lib, destroyHandle.c_str() );
            if ( dlerror() )
            {
                ERR ( "Cannot load symbol <" << destroyHandle << ">." );
                return NULL;
            }
            // Use symbols
            // Create loaded-object
            T *obj = create ( xml );
            // Store pointers for destroying an loaded-object
            obj->clean = destroy;
            obj->lib = lib;
            return obj;
        }

};

#endif
