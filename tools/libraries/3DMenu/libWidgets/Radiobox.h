#ifndef _RADIOBOX_H_
#define _RADIOBOX_H_

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
#include "SceneGraph.h"
#include <string>
#include <list>

class Radiobox : public Widget
{
    private:
        // This field contains the model for the checked marking
        SceneGraph *checked;
        // This field holds the animation-name.
        std::string *animationName;
        // This field holds the callback name
        std::string *callbackSelected;
        std::string *callbackDeselected;
        // This field contains the group name of multiple radioboxes
        std::string *groupName;
        // This function is used to parse a symbol for the rendered button
        void parse_symbol(char *path, char symbol);
        // This field is true, if the checkbox is selected
        bool selected;
        std::string radiobox_name;

    public:

        /**
         * The constructor loads the widget shape and the animation-name.
         * @param xml Pass an XMLNode -object, which focuses on the right XML-node.
         **/
        Radiobox ( XMLNode *xml );


        void setSelection(bool selection);

        /**
         * This is just a dummy implementation.
         **/
        void pressed();

        /**
         * This is just a dummy implementation.
         **/
        void released();

};

class RadioboxItem{
    public:
        std::string name;
        std::string group;
        Radiobox *box;
        RadioboxItem(std::string n, std::string g, Radiobox *b){
            name = n;
            group = g;
            box = b;
        }
};

class RadioboxMaster{
    private:
        static std::list<RadioboxItem*> radiobox_list;
    public:
        static void registerRadiobox(Radiobox *box, std::string name, std::string group);
        static void selectedRadiobox(std::string name, std::string group);
};

#endif
