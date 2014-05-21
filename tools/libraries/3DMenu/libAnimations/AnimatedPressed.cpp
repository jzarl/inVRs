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

#include "AnimatedPressed.h"

AnimatedPressed::AnimatedPressed ( XMLNode *xml ) : Animation ( xml )
{
    difference = xml->getAttributeValueAsFloat ( "difference", 0.35 );
    speed = xml->getAttributeValueAsFloat ( "speed", 2 );
}

void AnimatedPressed::animate ( MenuObject *obj )
{
    ThreadQueue::addToThreadQueue ( new AnimationThread ( obj, -difference, -speed ) );
}

void AnimatedPressed::unanimate ( MenuObject *obj )
{
    ThreadQueue::addToThreadQueue ( new AnimationThread ( obj, difference, speed ) );
}

bool AnimatedPressed::AnimationThread::run ( int time )
{
    if ( this->time != 0 )
    {
        float move = ( static_cast<float> ( time - this->time ) / 1000 ) * speed;
        if ( fabs ( move ) < fabs ( difference ) )
        {
            obj->getSceneGraph()->setTranslate ( 0, 0, move );
            difference -= move;
        }
        else
        {
            obj->getSceneGraph()->setTranslate ( 0, 0, difference );
            return false;
        }
    }
    this->time = time;
    return true;
}

AnimatedPressed::AnimationThread::AnimationThread ( MenuObject *obj, float difference, float speed )
        : obj ( obj ), difference ( difference ), speed ( speed ), time ( 0 ) {}

// The class factories

extern "C" AnimatedPressed* _create_AnimatedPressed ( XMLNode *xml )
{
    return new AnimatedPressed ( xml );
}

extern "C" void _destroy_AnimatedPressed ( AnimatedPressed* ani )
{
    delete ani;
}
