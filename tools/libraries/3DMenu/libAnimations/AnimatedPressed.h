#ifndef _ANIMATEDPRESSED_H_
#define _ANIMATEDPRESSED_H_

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
 * \brief This class provides a continuously widget-moving-animation.
 *
 * This class animates widgets. The animation can be used for moving
 * a button when someone triggers a pressed- or released-event.
 *
 * The animation moves continuously the widget along the z-axis. The distance
 * and the speed is ajustable.
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
 * @date 9th of May 2007
 * @author Thomas Weberndorfer
 *
 **/
class AnimatedPressed : public Animation
{

    private:

        /*
         * This class does the real animation. It uses the QueuedThread -class as
         * superclass, because the animation should be done by the ThreadQueue -class.
         */
    class AnimationThread : public QueuedThread
        {

            private:

                // reference to the widget, which should be animated
                MenuObject *obj;
                // animation-parameters
                float difference, speed;
                // timestamp of last animation-step
                int time;

            public:

                // the constructor stores a few variables
                AnimationThread ( MenuObject *obj, float difference, float speed );
                // This method is called by the ThreadQueue -class and performs the animation.
                bool run ( int time );

        };

        // animation-parameters
        float difference, speed;

    public:

        /**
         * The constructor loads the animation-parameters from the XML-file
         * @param xml This object has to focus on the corrent XML-node.
         **/
        AnimatedPressed ( XMLNode *xml );

        /**
         * This method creats an object, which performs the animation.
         * @param obj This is the widget, which will be animated.
         **/
        void animate ( MenuObject *obj );

        /**
         * This method creats an object, which cancels the animation-effects.
         * @param obj This is the widget, which will be animated.
         **/
        void unanimate ( MenuObject *obj );

};

#endif
