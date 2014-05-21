#ifndef _BUTTON_H_
#define _BUTTON_H_

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

#include "Widget.h"
#include <string>

/**
 * \brief This is a dummy-class for testing reasons
 *
 * This class is just a dummy implementation for testing the
 * library-loading-mechanism and a few other concepts.
 * It is a sub-class of Widget and is going to be extended to
 * the "real" Button -class.
 *
 * The constructor loads a VRML-file and uses the inherited methods
 * for placing it at the right position. It also loads an animation for
 * pressing and releasing the button.
 *
 * The important factory-Methods, which are defined in the cpp-file, are:
 *
 *   <code>extern "C" Widget* _create_Button(XMLNode *xml) {</code>
 *   <code>   return new Button(xml);                       </code>
 *   <code>}                                                </code>
 *
 *   <code>extern "C" void _destroy_Button(Widget* w) {     </code>
 *   <code>  delete w;                                      </code>
 *   <code>}                                                </code>
 *
 * The word "Button" in the method-signatures has to be equal to the
 * classname!
 *
 * @date 24th of March 2007
 * @author Thomas Weberndorfer
 *
 **/
class Button : public Widget
{

    private:

        // This field holds the animation-name.
        std::string *animationName;
        // This field holds the callback name
        std::string *callbackPressed;
        std::string *callbackReleased;
        // This function is used to parse a symbol for the rendered button
        void parse_symbol(char *path, char symbol);

    public:

        /**
         * The constructor loads the widget shape and the animation-name.
         * @param xml Pass an XMLNode -object, which focuses on the right XML-node.
         **/
        Button ( XMLNode *xml );

        /**
         * This is just a dummy implementation.
         **/
        void pressed();

        /**
         * This is just a dummy implementation.
         **/
        void released();

};

#endif
