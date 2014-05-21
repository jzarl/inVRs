#ifndef _BAR_H_
#define _BAR_H_

/***************************************************************************
 *   Copyright (C) 2007 by Thomas Webernorfer   *
 *   thomas_weberndorfer@hotmail.com   *
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

#include "Widget.h"
#include <string>

class Bar : public Widget
{

    private:

        // This field holds the animation-name.
        std::string *animationName;
        // This function is used to parse a symbol for the text on the titlebar
        void parse_symbol(char *path, char symbol);

    public:

        /**
         * The constructor loads the widget shape and the animation-name.
         * @param xml Pass an XMLNode -object, which focuses on the right XML-node.
         **/
        Bar ( XMLNode *xml );

        void pressed();

        void released();

};

#endif
