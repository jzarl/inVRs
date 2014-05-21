#ifndef _FADEDVISIBILITY_H_
#define _FADEDVISIBILITY_H_

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
 * \brief This class fades a widget in and out.
 *
 * This animation changes the visibility of a widget. The
 * visibility is slowly modified by increasing or
 * decreasing the tranparency.
 *
 * The method animate( MenuObject *obj ) makes a widget visible.
 * The method unanimate( MenuObject *obj) makes a widget invisible.
 *
 * This animation uses a continuously transparency change, which is
 * simulated by many very small steps. This cannot be done directly
 * in this class, because the animate( MenuObject *obj ) and
 * unanimate( MenuObject *obj ) methods have to return very quickly.
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
class FadedVisibility : public Animation
{

    private:

        /*
         * This class does the real animation. It uses the QueuedThread -class as
         * superclass, because the animation should be done by the ThreadQueue -class.
         */
    class FadeThread : public QueuedThread
        {

            private:

                // reference to the widget, which should be animated
                MenuObject *obj;
                // this indicates the direction of the animation (fade-in = true/fade-out = false)
                bool direction;
                // the current alpha value of the widget
                float alpha;
                // timestamp of last animation-step
                int time;
                // this is the speed of the transparency change [delta alpha / second]
                float transparencyChangeSpeed;

            public:

                // the constructor stores a few variables
                FadeThread ( MenuObject *obj, bool direction, float transparencyChangeSpeed );
                // This method is called by the ThreadQueue -class and performs the animation.
                bool run ( int time );

        };

        // animation parameter
        float transparencyChangeSpeed;

    public:

        /**
         * The constructor loads the animation-parameters from the XML-file
         * @param xml This object has to focus on the corrent XML-node.
         **/
        FadedVisibility ( XMLNode *xml );

        /**
         * This method creats an object, which performs the animation.
         * @param obj This is the widget, which will become visible.
         **/
        void animate ( MenuObject *obj );

        /**
         * This method creats an object, which cancels the animation-effects.
         * @param obj This is the widget, which will become invisible.
         **/
        void unanimate ( MenuObject *obj );

};

#endif
