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

#include "Menu.h"
#include "Config.h"

Menu::Menu ( XMLNode *xml ) : MenuObject ( xml )
{
    // place the menu
    float posx =    xml->getAttributeValueAsFloat ( "xpos", 0 );
    float posy =    xml->getAttributeValueAsFloat ( "ypos", 0 );
    float posz =    xml->getAttributeValueAsFloat ( "zpos" ,0 );
    float pitch =   xml->getAttributeValueAsFloat ( "pitch" ,0 );
    float heading = xml->getAttributeValueAsFloat ( "heading", 0 );
    getSceneGraph()->setRotate ( 1, 0, 0, pitch );
    getSceneGraph()->setRotate ( 0, 1, 0, heading );
    getSceneGraph()->setTranslate ( posx, posy, posz );
}

Menu::~Menu()
{
    // Call the Widget::deleteWidget function for all widgets in the store.
    store.removeAll ( Widget::deleteObject );
}

Widget* Menu::getWidget ( const char* name )
{
    // retrieve a widget-pointer
    return store.get ( name );
}

void Menu::addWidget ( Widget *child )
{
    if ( store.put ( child->getName(), child ) )
    {
        // storing was successful - now merge the scene
        INF ( "Widget <" << child->getName() << "> added to < " << getName() << ">." );
        getSceneGraph()->mergeScene ( child->getSceneGraph() );
    }
}
