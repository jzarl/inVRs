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

#include "SimpleVisibility.h"

SimpleVisibility::SimpleVisibility ( XMLNode *xml ) : Animation ( xml ) {};

void SimpleVisibility::animate ( MenuObject *obj )
{
    obj->getSceneGraph()->setVisibility ( true );
}

void SimpleVisibility::unanimate ( MenuObject *obj )
{
    obj->getSceneGraph()->setVisibility ( false );
}

// The class factories

extern "C" SimpleVisibility* _create_SimpleVisibility ( XMLNode *xml )
{
    return new SimpleVisibility ( xml );
}

extern "C" void _destroy_SimpleVisibility ( SimpleVisibility* ani )
{
    delete ani;
}
