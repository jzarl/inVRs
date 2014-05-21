#ifndef _MENUCONTROL_H_
#define _MENUCONTROL_H_

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

#include "Menu.h"
#include "ObjectStore.h"
#include "SceneGraph.h"

/**
 * \brief This class controlls the 3dMenu.
 *
 * The setup of the 3dMenu is defined by an XML-file. This class
 * provides static methods for loading these XML-files and modifying
 * the loaded menus and widgets.
 *
 * This class handles also the input-events, which should be passed to the
 * 3dMenu-library, using the method MenuControl::menuButton( int state, Ray &ray).
 *
 * The method MenuControl::getUniversalRootPtr() returns the
 * root-node of all menus, which has to be added to the application scenegraph.
 * The coordinate-system of the node, to which the menu-root-node has been added,
 * has to match with the coordinate-system of the parameter ray of the method
 * void MenuControl::menuButton( int state, Ray &ray).
 *
 * @date 30rd of April 2007
 * @author Thomas Weberndorfer
 *
 **/
class MenuControl
{

    private:

        // This object stores references to the menues managed by the MenuControl.
        static ObjectStore<Menu> menus;
        // This is the root-SceneGraph, which holds all menus.
        static SceneGraph *scene;
        // This is a simple helper-method for adding a menu to the SceneGraph -object
        // and the ObjectStore -object.
        static void addMenu ( Menu *m );

    public:

        /**
         * This method removes all the menus and widgets.
         **/
        static void deleteAllMenus();

        /**
         * This method removes the menu and all its widgets with the name equal to the
         * parameter name.
         * @param name The menu with this name will be removed including all widgets.
         **/
        static void deleteMenu ( const char* name );

        /**
         * This method returns the parent-node of all menus.
         * @return the universal parent node
         **/
        static SGPtr getUniversalRootPtr();

        /**
         * The setup of a menu is stored in an XML-file. This method loads such XML-files
         * and creates the menus.
         * @param xmlFile the XML-file containing the menu.
         **/
        static void loadMenu ( const char* xmlFile );

        /**
         * This method handles input events.
         * @param state The state indicates the position of the menu-button of the input-device.
         *              (0 means pressed and 1 means released)
         * @param ray   This ray will be intersected with the widgets. The first hit widget gets the
         *              pressed- and released-events.
         **/
        static void menuButton ( int state, Ray &ray );

        /**
         * This method makes a widget visible or unvisible.
         * @param menu      the name of the menu which contains the widget
         * @param widget    the name of the widget
         * @param show      true makes the widget visible, false makes it invisible
         **/
        static void showWidget ( const char* menu, const char* widget, bool show );

        /**
         * This method makes a menu visible or unvisible.
         * @param menu      the name of the menu
         * @param show      true makes the menu visible, false makes it invisible
         **/
        static void showMenu ( const char* menu, bool show );

        /**
         * @author: RLANDER
         * This method returns a pointer to a menu instance matching to the passed name
         * @param menu      the name of the menu
         * @return          pointer to the menu
         */
        static Menu* getMenu ( const char* menu );

};

#endif
