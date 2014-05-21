#ifndef _SIMPLEPRESSED_H_
#define _SIMPLEPRESSED_H_

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
 * \brief This class provides a widget-displacing-animation.
 *
 * This class animates widgets. The animation can be used for displacing
 * a button when someone triggers a pressed- or released-event.
 *
 * The animation displaces the widget along the z-axis. The distance
 * is  ajustable.
 *
 * @date 8th of May 2007
 * @author Thomas Weberndorfer
 *
 **/
class SimplePressed : public Animation
{

    private:

        // This is the displacing-distance along the x-axis.
        float distance;

    public:

        /**
         * The constructor loads the distance for displacing the widget from
         * the XML-file.
         * @param xml This XML-object has to focus on the correct node.
         **/
        SimplePressed ( XMLNode *xml );

        /**
         * This method performs the displacement.
         * @param obj This is a pointer to the widget, which should be animated.
         **/
        void animate ( MenuObject *obj );

        /**
         * This method cancels the displacement and puts the widget
         * back to its origin position.
         * @param obj This is a pointer to the widget, which should be animated.
         **/
        void unanimate ( MenuObject *obj );

};

#endif
