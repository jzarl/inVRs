#ifndef _MENU_H_
#define _MENU_H_

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
#include "ObjectStore.h"

/**
 * \brief This class groups widgets into menus.
 *
 * The Menu -class is responsible for managing the menus. It stores references to
 * all added widgets.
 *
 * The menu can be moved, rotated, scaled, etc. by using the methods provided
 * by the SceneGraph -class. Destroying a menu is done by simply deleting it.
 * The destructor of the Menu -class removes automatically all added widgets from
 * memory and from the scenegraph.
 *
 * The creation of menus and widgets is done by the MenuControl -class.
 *
 * @date 24th of March 2007
 * @author Thomas Weberndorfer
 *
 * <b>Updates:</b>
 *
 * - The menus are now created by the MenuControl -class. (23rd of April 2007)
 *
 **/
class Menu : public MenuObject
{

    private:

        // This ObjectStore holds names and references of the widgets.
        ObjectStore <Widget>store;

    public:

        /**
         * The constructor creates a new menu.
         **/
        Menu ( XMLNode *xml );

        /**
         * The destructor deletes all Widgets, which have been added to this menu.
         **/
        ~Menu();

        /**
         * This method adds a widget to the menu.
         * @param child This is the Widget, which should be added.
         **/
        void addWidget ( Widget *child );

        /**
         * This method allows retrieving a pointer to a Widget with the
         * name equally the parameter name.
         * @param name This is the name of the Widget to whom a pointer is needed.
         * @return This method returns a pointer to the Widget with the given name,
         *         or NULL if such a Widget doesn't exist in this menu.
         **/
        Widget* getWidget ( const char* name );

};

#endif
