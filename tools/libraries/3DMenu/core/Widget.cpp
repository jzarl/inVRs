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

#include "Widget.h"

Widget::Widget ( XMLNode *xml ) : MenuObject ( xml )
{
    // Store a pointer to this Object in an attachement for picking
    getSceneGraph()->setPickingPointer ( this );
    // Place the widget.
    float posx = xml->getAttributeValueAsFloat ( "xpos", 0 );
    float posy = xml->getAttributeValueAsFloat ( "ypos", 0 );
    getSceneGraph()->setTranslate ( posx, -posy, 0 );
}
