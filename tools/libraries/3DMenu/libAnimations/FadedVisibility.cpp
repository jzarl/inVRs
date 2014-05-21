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

#include "FadedVisibility.h"

FadedVisibility::FadedVisibility ( XMLNode *xml ) : Animation ( xml )
{
    transparencyChangeSpeed = xml->getAttributeValueAsFloat ( "changeSpeed", 1 );
}

void FadedVisibility::animate ( MenuObject *obj )
{
    ThreadQueue::addToThreadQueue ( new FadeThread ( obj, true, transparencyChangeSpeed ) );
}

void FadedVisibility::unanimate ( MenuObject *obj )
{
    ThreadQueue::addToThreadQueue ( new FadeThread ( obj, false, transparencyChangeSpeed ) );
}

FadedVisibility::FadeThread::FadeThread ( MenuObject *obj,
        bool direction,
        float transparencyChangeSpeed ) : time ( 0 )
{
    this->obj = obj;
    this->direction = direction;
    this->transparencyChangeSpeed = transparencyChangeSpeed;
    // set starting value for alpha
    if ( direction )
    {
        // fade in
        alpha = 0;
    }
    else
    {
        // fade out
        alpha = 1;
    }
}

bool FadedVisibility::FadeThread::run ( int time )
{
    if ( this->time != 0 )
    {
        if ( direction )
        {
            // fade in
            alpha += 0.001 * static_cast<float> ( time - this->time ) * transparencyChangeSpeed;
            if ( alpha > 1 ) alpha = 1;
            obj->getSceneGraph()->setTransparency ( alpha );
            if ( alpha >= 1 ) return false;
        }
        else
        {
            // fade out
            alpha -= 0.001 * static_cast<float> ( time - this->time ) * transparencyChangeSpeed;
            if ( alpha <= 0 ) alpha = 0;
            obj->getSceneGraph()->setTransparency ( alpha );
            if ( alpha <= 0 ) return false;
        }
    }
    this->time = time;
    return true;
}

// The class factories

extern "C" FadedVisibility* _create_FadedVisibility ( XMLNode *xml )
{
    return new FadedVisibility ( xml );
}

extern "C" void _destroy_FadedVisibility ( FadedVisibility* ani )
{
    delete ani;
}
