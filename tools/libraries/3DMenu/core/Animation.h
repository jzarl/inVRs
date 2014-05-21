#ifndef _ANIMATION_H_
#define _ANIMATION_H_

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
#include "Loader.h"

/**
 * \brief This is the base-class of all Animation -objects.
 *
 * This is the base-class of all Animation -objects and provides the
 * mechanism for loading the Animation -objects from external *.so-files.
 * The needed information for loading the object is read from an
 * XML-file. For each "Animation"-tag an Animation -object will be
 * created and stored in the Registry, which provides methods
 * for invoking the animation-action.
 *
 * For each "Animation"-tag in the XML-file exists only one Animation -object.
 * So one Animation -object will be used by multiple widgets and has to be
 * thread-save.
 *
 * @date 4th of May 2007
 * @author Thomas Weberndorfer
 *
 **/
class Animation : public NamedObject, public Loader<Animation>
{

    protected:

        /**
         * The constructor is protected, because only the inherited create-method
         * is allowed to create Animation -objects.
         * @param xml This XMLNode -object has to focus on the right XML-node.
         **/
        Animation ( XMLNode *xml ) : NamedObject ( xml ) {}

    public:

        /**
         * This method animates an object. It is abstract,
         * because this class is just the base-class for Animation -objects.
         * @param obj The animation will be performed on this object.
         **/
        virtual void animate ( MenuObject *obj ) = 0;

        /**
         * This method has to undo the animation. It is abstract,
         * because this class is just the base-class for Animation -objects.
         * @param obj The animation will be performed on this object.
         **/
        virtual void unanimate ( MenuObject *obj ) = 0;

        /**
         * Nothing to do ...
         **/
        virtual ~Animation() {}
};

#endif
