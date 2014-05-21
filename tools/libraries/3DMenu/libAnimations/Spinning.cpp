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

#include "Spinning.h"

Spinning::SpinningThread::SpinningThread ( MenuObject *obj, float rotationspeed ) :
        time ( 0 ), completedangle ( 0 )
{
    this->rotationspeed = rotationspeed;
    this->obj = obj;
}

bool Spinning::SpinningThread::run ( int time )
{
    if ( this->time != 0 )
    {
        float angle = static_cast<float> ( time - this->time ) * 0.001 * rotationspeed;
        completedangle += angle;
        if ( completedangle > 360 )
        {
            // too much rotating
            angle -= ( completedangle - 360 );
            completedangle = 360;
        }
        obj->getSceneGraph()->setRotate ( 0,1,0, angle );
        if ( completedangle>=360 ) return false;
    }
    this->time = time;
    return true;
}

Spinning::Spinning ( XMLNode *xml ) : Animation ( xml )
{
    rotationspeed = xml->getAttributeValueAsFloat ( "rotationSpeed", 360 );
}

void Spinning::animate ( MenuObject *obj )
{
    ThreadQueue::addToThreadQueue ( new SpinningThread ( obj, rotationspeed ) );
}

void Spinning::unanimate ( MenuObject *obj ) { /* nothing */}

// The class factories

extern "C" Spinning* _create_Spinning ( XMLNode *xml )
{
    return new Spinning ( xml );
}

extern "C" void _destroy_Spinning ( Spinning* ani )
{
    delete ani;
}
