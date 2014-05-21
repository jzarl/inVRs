#ifndef _NAMEDOBJECT_H_
#define _NAMEDOBJECT_H_

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

#include "XMLNode.h"
#include <string>

/**
 * \brief This class creates named objects.
 *
 * Many parts of the 3dMenu-library depend on named objects. The names
 * are stored in an XML-file. This class provides the functionality for
 * reading and storing these names.
 *
 * This class should be used as base-class. Direct object instantiation is
 * NOT intended. Therefore, this class has a protected constructor.
 *
 * @date 23rd of March 2007
 * @author Thomas Weberndorfer
 *
 **/
class NamedObject
{

    private:

        // This variable holds the name of the MenuObject.
        std::string name;

        // The counter is used to generate unique names
        // in case of a missing name in the XML-file.
        static int counter;

    protected:

        /**
         * The constructor sets the name of this object.
         * @param xml This XMLNode -object has to contain the needed name.
         **/
        NamedObject ( XMLNode *xml );

    public:

        /**
         * This method returns the name, which has been
         * retrieved from the XML-file.
         * @return the object-name
         **/
        const char* getName();

};

#endif
