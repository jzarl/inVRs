#ifndef _OBJECTSTORE_H_
#define _OBJECTSTORE_H_

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

#include <map>
#include "Config.h"

/**
 * \brief This is a template class for storing key-value-pairs.
 *
 * This class stores key-value-pairs. The type of the keys is always
 * the STL string-type. The type of the values can be set through
 * the template instantiation.
 *
 * This class excepts only key-value-pairs with unique keys. Duplicate
 * key-value-pairs will be ignored.
 *
 * This class offers methods for storing and retrieving key-value-pairs.
 * There are also special methods for deleting key-value-pairs. A method-pointer
 * can be used, to call a method before the deletion-process begins.
 *
 * @date 23rd of March 2007
 * @author Thomas Weberndorfer
 *
 **/
template <typename T> class ObjectStore
{

    private:

        // the STL map stores all the items
        std::map<std::string, T*> store;

    public:

        /**
         * This method deletes the value of a key-value-pair.
         * @param value The value to which this pointer points, will be deleted.
         **/
        static void _deleteAlsoContent_ ( T* value )
        {
            delete value;
        }

        /**
         * This method puts a key-value-object into the store.
         * @param key   the key 
         * @param value the value
         * @return true on success, false in any other cases
         **/
        bool put ( const char* key, T* value )
        {
            // test if key exists already
            typename std::map<std::string, T*>::iterator iter = store.find ( key );
            if ( iter == store.end() )
            {
                // key is new
                store[key] = value;
                return true;
            }
            else
            {
                // key is used already
                WAR ( "Duplicate name <" << key << "> will be ignored!" );
                return false;
            }
        }

        /**
         * This method retrieves a value from the store with the fitting key.
         * @param key This is the key of the needed key-value-pair.
         * @return the value or NULL if the needed key-value-pair doesn't exist
         *         in the store
         **/
        T* get ( const char* key )
        {
            // search key-value-pair
            typename std::map<std::string, T*>::iterator iter = store.find ( key );
            if ( iter == store.end() )
            {
                // key not found
                INF ( "Key-value-pair with key <" << key << "> not found!" );
                return NULL;
            }
            else
            {
                // key-value-pair found - return value
                return iter->second;
            }
        }

        /**
         * This method removes an item from the store. But first the parameter
         * method will be invoked.
         * @param key This is the key of the key-value-pair, which should be removed.
         * @param method This method will be invoked. If no method is needed,
         *               use NULL.
         **/
        void remove ( const char* key, void ( *method ) ( T* ) )
        {
            // search key-value-pair
            typename std::map<std::string, T*>::iterator iter = store.find ( key );
            if ( iter != store.end() )
            {
                // key found - call the method
                if ( method != NULL ) method ( iter->second );
                // delete key-value-pair
                store.erase ( iter );
            }
            else
            {
                // key does not exist
                INF ( "Key-value-pair with key <" << key << "> not found!" );
            }
        }

        /**
         * This method removes all items in the store. But first the parameter
         * method will be invoked.
         * @param method This method will be invoked first. If no method is needed,
         *               use NULL.
         **/
        void removeAll ( void ( *method ) ( T* ) )
        {
            // iterate through all key-value-pairs
            typename std::map<std::string, T*>::iterator iter;
            for ( iter = store.begin(); iter != store.end() ; iter++ )
            {
                // invoke the method
                if ( method != NULL ) method ( iter->second );
            }
            // now delete the key-value-pairs
            store.clear();
        }

};

#endif
