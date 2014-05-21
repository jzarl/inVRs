#ifndef _MENUOBJECT_H_
#define _MENUOBJECT_H_

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

#include "SceneGraph.h"
#include "NamedObject.h"

/**
 * \brief This is the base class of all widgets and the menu.
 *
 * The MenuObject is the base class of all widgets and the menu. It performs
 * basic tasks like providing a SceneGraph -object and scaling it.
 *
 * The information for setting up the SceneGraph -object
 * is read from an XMLNode -object, which focuses on a specific node in
 * an XML-file.
 *
 * The animation for making a widget visible or invisible can be defined by
 * the attribute "visibilityanimation". If no animation has been asigned to this
 * attribute, the method setVisibility( bool vis ) has no effect.
 *
 * This class should be used as base-class. Direct object instantiation is
 * NOT intended. Therefore, this class has a protected constructor.
 *
 * @date 23rd of March 2007
 * @author Thomas Weberndorfer
 *
 * <b>Updates:</b>
 *
 * - removed all OSG-depending parts into the SceneGraph -class (26th of April 2007)
 * - moved the naming functionality into the class NamedObject (3rd of May 2007)
 * - the animation for changing the visibility can be defined per XML-file (15th of May 2007)
 *
 **/
class MenuObject : public NamedObject
{

    private:

        // This object stores the representation
        SceneGraph *scene;
        // This string stores the animation for changing the visibility
        std::string visibilityAnimation;
        // This variable stores the visibility-state
        bool visible;

    protected:

        /**
         * The constructor needs an XMLNode -object for setting up the base
         * properties.
         * @param xml Pass an XMLNode -object, which focuses on the correct
         *            XML-node in the XML-file.
         **/
        MenuObject ( XMLNode *xml );

    public:

        /**
         * The destructor deletes the SceneGraph -object.
         **/
        virtual ~MenuObject();

        /**
         * This method returns the SceneGraph -object.
         * @return the SceneGraph -object
         **/
        inline SceneGraph* getSceneGraph() { return scene; }

        /**
         * This method handels the visibility. The animation used for changing the
         * visibility can be described using the XML-file.
         * @param vis Set the visibility. true makes the MenuObject visible;
         *                                false makes the MenuObject invisible;
         **/
        void setVisibility ( bool vis );

};

#endif
