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

#include "SimplePressed.h"

SimplePressed::SimplePressed ( XMLNode *xml ) : Animation ( xml )
{
    distance = xml->getAttributeValueAsFloat ( "distance", 0.35 );
}

void SimplePressed::animate ( MenuObject *obj )
{
    if ( obj != NULL )
        obj->getSceneGraph()->setTranslate ( 0, 0, -distance );
}

void SimplePressed::unanimate ( MenuObject *obj )
{
    if ( obj != NULL )
        obj->getSceneGraph()->setTranslate ( 0, 0, distance );
}

// The class factories

extern "C" SimplePressed* _create_SimplePressed ( XMLNode *xml )
{
    return new SimplePressed ( xml );
}

extern "C" void _destroy_SimplePressed ( SimplePressed* ani )
{
    delete ani;
}
