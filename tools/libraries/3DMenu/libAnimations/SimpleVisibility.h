#ifndef _SIMPLEVISIBILITY_H_
#define _SIMPLEVISIBILITY_H_

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

#include "Animation.h"

/**
 * \brief This class makes a widget visible and invisible.
 *
 * This animation changes the visibility of a widget. The
 * visibility is changed at once.
 *
 * The method animate( MenuObject *obj ) makes a widget visible.
 * The method unanimate( MenuObject *obj) makes a widget invisible.
 *
 * @date 15th of May 2007
 * @author Thomas Weberndorfer
 *
 **/
class SimpleVisibility : public Animation
{

    public:

        /**
         * The constructor forewards the XMLNode -object.
         * @param xml This XML-object has to focus on the correct node.
         **/
        SimpleVisibility ( XMLNode *xml );

        /**
         * This method makes the vidget visible.
         * @param obj This is a pointer to the widget, which will become visible.
         **/
        void animate ( MenuObject *obj );

        /**
         * This method makes the vidget invisible.
         * @param obj This is a pointer to the widget, which will become invisible.
         **/
        void unanimate ( MenuObject *obj );

};

#endif
