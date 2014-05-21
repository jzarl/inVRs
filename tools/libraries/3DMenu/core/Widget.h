#ifndef _WIDGET_H_
#define _WIDGET_H_

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
 * \brief This class is the super-class of all widgets.
 *
 * The Widget class is the super-class of all widgets. The class itself is
 * abstract and so the sub-classes (widgets) have to implement the
 * missing methods.
 *
 * The widgets themselves are placed into separate libraries, which will be loaded
 * at runtime. So new widgets can be added to the 3dMenu-library without recompiling
 * a single line. The location of the library containing the widgets and the class-names
 * of the widgets, which should be loaded, must be defined in an XML-file.
 * The creation of a widget is done by the inherited static method
 * createObject(XMLNode *xml), which searches for the library and creates the demanded widget.
 *
 * The abstract Widget -class is a sub-class of the MenuObject -class and
 * provides all the usefull stuff like creating a SceneGraph -Object
 * and removing it from the global scene, if the object has been deleted.
 *
 * Widgets may use parts of the core-package like the Registry. But including the
 * Registry leads to troubles, because the core-package and the external library, which
 * stores the widget, whould contain the Registry.
 *
 * So three method-pointers are used to avoid these problem, which are initialized
 * with the method void
 * registerCallbacks(RegistryAnimation animate, RegistryAnimation unanimate, RegistryCallBack callback ).
 *
 * <b>Attention:</b>
 *
 * Because the widgets are created by library calls, simply deleting the widgets with
 * <code>delete</code> may lead to crashes. Therefore the static
 * method deleteObject(Widget *w) must be used for removing widgets.
 *
 * @date 23rd of March 2007
 * @author Thomas Weberndorfer
 *
 * <b>Updates:</b>
 *
 * - removed all OSG-depending parts into the SceneGraph -class (26th of April 2007)
 * - moved loading mechanism into the seperate class-file Loader (3rd of May 2007)
 * - added pointers to the static methods of the Registry (6th May 2007)
 *
 **/
class Widget : public MenuObject, public Loader<Widget>
{

    protected:

        /**
         * The constructor is used to load the XMLNode -data for setting up the transformation,
         * name, library-path, classname, etc. of the widget. Therefore the XMLNode -object
         * has to focus on the correct XML-node in the XML-file.
         *
         * This constructor is protected, because only the inherited factory-method
         * Widget* createObject(XMLNode *xml) is allowed to call it.
         *
         * @param xml Use an XMLNode -object, which focuses on the correct node.
         **/
        Widget ( XMLNode *xml );

        /**
         * This is the type-definition for method-pointers with the type of
         * Registry::callAnimate and Registry::callUnAnimate.
         **/
        typedef void ( *RegistryAnimation ) ( MenuObject *obj, const char* symbol );

        /**
         * This is the type-definition for method-pointers with the type of
         * Registry::callCallBack.
         **/
        typedef void ( *RegistryCallBack ) ( const char* name, const char* value );

        /**
         * Use this method pointer instead of Registry::callAnimate!
         **/
        RegistryAnimation callAnimate;

        /**
         * Use this method pointer instead of Registry::callUnAnimate!
         **/
        RegistryAnimation callUnAnimate;

        /**
         * Use this method pointer instead of Registry::callCallBack!
         **/
        RegistryCallBack callCallBack;

    public:

        /**
         * The destructor has nothing to do ...
         **/
        virtual ~Widget() {}

        /**
         * This method initializes the method-pointers to the static
         * methods of the Registry -class.
         * @param animate pointer to Registry::callAnimate
         * @param unanimate pointer to Registry::callUnAnimate
         * @param callback pointer to Registry::callCallBack
         **/
        void registerCallbacks ( RegistryAnimation animate,
                                 RegistryAnimation unanimate,
                                 RegistryCallBack callback )
        {
            callAnimate = animate;
            callUnAnimate = unanimate;
            callCallBack = callback;
        }

        /**
         * This method will be call, when a witget has been pressed by the user.
         **/
        virtual void pressed() = 0;

        /**
         * This method will be call, when a witget has been released by the user.
         **/
        virtual void released() = 0;

};

#endif
