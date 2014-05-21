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

#include "Registry.h"
#include "Config.h"

ObjectStore<CallBack> Registry::callbackStore;
ObjectStore<Animation> Registry::animationStore;

void Registry::registerCallBack ( CallBack *callback, const char* symbol )
{
    if ( callbackStore.put ( symbol, callback ) )
    {
        // Key is new, that's OK
        INF ( "Symbol <" << symbol << "> stored in Registry." );
    }
}

void Registry::callCallBack ( const char* name, const char* value )
{
    CallBack *cb = callbackStore.get ( name );
    if ( cb != NULL )
    {
        // Key found, invoke callback
        cb->invoke ( name, value );
    }
}

void Registry::unregisterCallBack ( const char* symbol )
{
    callbackStore.remove ( symbol, callback );
}

void Registry::callback ( CallBack *cb )
{
    // Delete the container-objects, which wrapped the static callback-methods.
    Container *con = dynamic_cast<Container*> ( cb );
    if ( con != NULL ) delete con;
}

void Registry::registerAnimation ( Animation *ani, const char* symbol )
{
    if ( ani != NULL && animationStore.put ( symbol, ani ) )
    {
        // Key is new, that's OK
        INF ( "Symbol <" << symbol << "> stored in Registry." );
    }
}

void Registry::callAnimate ( MenuObject *obj, const char* symbol )
{
    Animation *ani = animationStore.get ( symbol );
    if ( ani != NULL )
    {
        // Animation found
        ani->animate ( obj );
    }
}

void Registry::callUnAnimate ( MenuObject *obj, const char* symbol )
{
    Animation *ani = animationStore.get ( symbol );
    if ( ani != NULL )
    {
        // Animation found
        ani->unanimate ( obj );
    }
}
