#ifndef _SPINNING_H_
#define _SPINNING_H_

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
#include "ThreadQueue.h"

/**
 * \brief This class rotates a widget by one turn.
 *
 * This method rotates a widget. The rotation-speed can be set by the XML-file.
 * The method void animate( MenuObject *obj ) performs a full rotation.
 * The method unanimate ( MenuObject *obj ) has nothing to do, because
 * the method void animate( MenuObject *obj ) leaves the widget in its
 * origin position already.
 *
 * This animation uses a continuously movement, which is simulated by many
 * very small movement steps. This cannot be done directly in this class,
 * because the animate( MenuObject *obj ) and unanimate( MenuObject *obj )
 * methods have to return very quickly.
 * So instead of animating the widget, a new object is created, which
 * uses QueuedThread as superclass. This new object is put into the
 * ThreadQueue -class, which performs the animation in an own thread. The
 * ThreadQueue -class deletes this object automatically, when the animation
 * has finished.
 *
 * @date 18th of May 2007
 * @author Thomas Weberndorfer
 *
 **/
class Spinning : public Animation
{

    private:

        /*
         * This class does the real animation. It uses the QueuedThread -class as
         * superclass, because the animation should be done by the ThreadQueue -class.
         */
    class SpinningThread : public QueuedThread
        {

            private:

                // reference to the widget, which should be animated
                MenuObject *obj;
                // timestamp of last animation-step
                int time;
                // speed of the rotation [degrees per second]
                float rotationspeed;
                // angle-sum of rotation steps
                float completedangle;

            public:

                // the constructor sets a few variables
                SpinningThread ( MenuObject *obj, float rotationspeed );

                // This method is called by the ThreadQueue -class and performs the animation.
                bool run ( int time );

        };

        // animation parameter; read from the XML-file
        float rotationspeed;

    public:

        /**
         * The constructor forewards the XMLNode -object and loads a few parameters.
         * @param xml This XML-object has to focus on the correct node.
         **/
        Spinning ( XMLNode *xml );

        /**
         * This method rotates the widget by one turn.
         * @param obj This is a pointer to the widget, which should be rotated.
         **/
        virtual void animate ( MenuObject *obj );

        /**
         * This method does nothing, because the animate( MenuObject *obj ) -method leaves the widget
         * in its default position already.
         * @param obj This is a pointer to the widget.
         **/
        virtual void unanimate ( MenuObject *obj );

};

#endif
