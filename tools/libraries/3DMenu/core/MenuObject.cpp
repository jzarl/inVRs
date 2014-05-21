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

#include "MenuObject.h"
#include "Registry.h"

MenuObject::MenuObject ( XMLNode *xml ) :  NamedObject ( xml ),  scene ( new SceneGraph() )
{
    const char *tmp;
    // Scale node
    float scale = xml->getAttributeValueAsFloat ( "scalexyz", 1 );
    scene->setScale ( scale, scale, scale );
    // Search for the visibility animation
    tmp = xml->getAttributeValue ( "visibilityanimation" );
    if ( tmp != NULL )
    {
        visibilityAnimation = tmp;
    }
    // Set Visibility
    tmp = xml->getAttributeValue ( "visible" );
    if ( tmp != NULL && !strcmp ( "false", tmp ) )
    {
        scene->setVisibility ( false );
        visible = false;
    }
    else
    {
        visible = true;
    }
}

void MenuObject::setVisibility ( bool vis )
{
    if ( vis )
    {
        if ( !visible )
        {
            Registry::callAnimate ( this, visibilityAnimation.c_str() );
            visible = true;
        }
    }
    else
    {
        if ( visible )
        {
            Registry::callUnAnimate ( this, visibilityAnimation.c_str() );
            visible = false;
        }
    }
}

MenuObject::~MenuObject()
{
    delete scene;
}
