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

#include "irrXML.h"
//#include "irrXML.cpp"
#include "Registry.h"
#include "Config.h"
#include "MenuControl.h"

ObjectStore<Menu> MenuControl::menus;
SceneGraph *MenuControl::scene = NULL;

void MenuControl::addMenu ( Menu *m )
{
    if ( scene == NULL ) scene = new SceneGraph();
    scene->mergeScene ( m->getSceneGraph() );
    menus.put ( m->getName(), m );
}

void MenuControl::deleteAllMenus()
{
    menus.removeAll ( ObjectStore<Menu>::_deleteAlsoContent_ );
}

void MenuControl::deleteMenu ( const char* name )
{
    menus.remove ( name, ObjectStore<Menu>::_deleteAlsoContent_ );
}

SGPtr MenuControl::getUniversalRootPtr()
{
    if ( scene == NULL ) scene = new SceneGraph();
    return scene->getNodePtr();
}

void MenuControl::loadMenu ( const char* xmlFile )
{
    irr::io::IrrXMLReader *xml = irr::io::createIrrXMLReader ( xmlFile );
    XMLNode *xmlnode = new XMLNode ( xml );
    int nodeCounter = 0;
    Menu *menu = NULL;
    // Parse the XML-file
    while ( xml && xml->read() )
    {
        // Check the node-type
        // Only a start-tag makes sense. ( <.....> or <..../> )
        if ( xml->getNodeType() == irr::io::EXN_ELEMENT )
        {
            nodeCounter++;
            // Check the node name
            if ( !strcmp ( "Menu", xml->getNodeName() ) )
            {
                // The node Menu
                const char* name = xmlnode->getAttributeValue ( "name" );
                if ( name == NULL )
                {
                    // The name-attribute is missing!
                    WAR ( "Node <Menu> without a name found! This xml-node and all subnodes will be ignored!" );
                    menu = NULL;
                }
                else
                {
                    // Create a new menu
                    menu = new Menu ( xmlnode );
                    addMenu ( menu );
                }
            }
            else if ( !strcmp ( "Widget", xml->getNodeName() ) )
            {
                // The node describes a widget.
                if ( menu != NULL )
                {
                    Widget *widget = Widget::createObject ( xmlnode );
                    // check the result
                    if ( widget != NULL )
                    {
                        // set method-pointers
                        widget->registerCallbacks ( Registry::callAnimate, Registry::callUnAnimate, Registry::callCallBack );
                        menu->addWidget ( widget );
                    }
                    // Reset the internal state of the method xmlnode->readNextSubNode().
                    while ( xmlnode->readNextSubNode() ) /* nothing */;
                }
                else
                {
                    WAR ( "Nodes of type <Widget> must be nested in nodes of type <Bar>!" );
                }
            }
            else if ( !strcmp ( "Animation", xml->getNodeName() ) )
            {
                Registry::registerAnimation ( Animation::createObject ( xmlnode ), xml->getAttributeValue ( "name" ) );
            }
        }
    }
    delete xmlnode;
    delete xml;
    // Print parsing-result
    if ( nodeCounter == 0 )
    {
        ERR ( "XML-file <" << xmlFile << "> could not be found or contains no XML-Nodes!" );
    }
    else if ( menu == NULL )
    {
        ERR ( "The file <" << xmlFile << "> should contain one valid node of type <Menu> at least!" );
    }
    else
    {
        INF ( "Parsing completed: " << nodeCounter << " nodes found in xml-file <" << xmlFile << ">." );
    }
}

void MenuControl::menuButton ( int state, Ray &ray )
{
    void *obj = scene->getPickingPointer ( ray );
    // get a Widget-pointer
    // TODO: make it save
    static Widget *w;
    if ( !state ) w = reinterpret_cast<Widget*> ( obj );
    if ( w != NULL )
    {
        // Widget hit
        if ( state ==  0 )
        {
            // Menu-button is pressed
            w->pressed();
        }
        else
        {
            // Menu-button has been released
            w->released();
            w = NULL;
        }
    }
}

void MenuControl::showWidget ( const char* menu, const char* widget, bool show )
{
    Menu *m = menus.get ( menu );
    if ( m != NULL )
    {
        // menu found
        Widget *w = m->getWidget ( widget );
        if ( w != NULL ) w->setVisibility ( show );
    }
}

void MenuControl::showMenu ( const char* menu, bool show )
{
    Menu *m = menus.get ( menu );
    if ( m != NULL ) m->setVisibility ( show );
}

// @RLANDER: added method to get access to menu
Menu* MenuControl::getMenu ( const char* menu )
{
	return menus.get ( menu );
}
