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

#include <OpenSG/OSGGLUT.h>
#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGSimpleSceneManager.h>
#include "MenuControl.h"
#include "Registry.h"
#include <stdlib.h>

OSG_USING_NAMESPACE;

SimpleSceneManager *mgr;

int setupGLUT ( int *argc, char *argv[] );

//Registry test - this should be done by the application, which uses the 3dMenu.
class testcallback: public CallBack {
public:
    void invoke ( const char *name, const char *value ) {
        std::cout << "(II) TestCallback with name <" << name << "> and value <"
        << value << ">"  << std::endl;
    }
};

//Registry test - this should be done by the application, which uses the 3dMenu.
void statictestcallback ( const char *name, const char *value ) {
    std::cout << "(II) TestCallback with name <" << name << "> and value <"
    << value << ">"  << std::endl;
}

int main ( int argc, char **argv ) {
    osgInit ( argc,argv );
    int winid = setupGLUT ( &argc, argv );
    GLUTWindowPtr gwin= GLUTWindow::create();
    gwin->setId ( winid );
    gwin->init();
    //
    mgr = new SimpleSceneManager;
    MenuControl::loadMenu ( "testapplication/Menu.xml" );
    mgr->setWindow ( gwin );
    mgr->setRoot ( MenuControl::getUniversalRootPtr() );
    // Registry test
    // This should be done by the application, which uses the 3dMenu.
    Registry::registerCallBack ( new testcallback(), "Button1" );
    Registry::registerCallBack ( statictestcallback, "Radiobox1" );
    Registry::registerCallBack ( statictestcallback, "Radiobox2" );
    Registry::registerCallBack ( statictestcallback, "Radiobox3" );
    //
    glutMainLoop();
    return 0;
}

void display ( void ) {
    mgr->redraw();
}

void reshape ( int w, int h ) {
    mgr->resize ( w, h );
    glutPostRedisplay();
}

void mouse ( int button, int state, int x, int y ) {
    Line l = mgr->calcViewRay ( x, y );
    if(button == 0)
        MenuControl::menuButton ( state, l );
    else if ( state ) {
        mgr->mouseButtonRelease ( button, x, y );
    } else {
        mgr->mouseButtonPress ( button, x, y );
    }
    glutPostRedisplay();
}

void motion ( int x, int y ) {
    mgr->mouseMove ( x, y );
    glutPostRedisplay();
}

void keyboard ( unsigned char k, int x, int y ) {
    switch ( k ) {
    case 27: {
            OSG::osgExit();
            exit ( 0 );
        }
        break;
    case 'q': {
            MenuControl::deleteAllMenus();
            mgr->redraw();
        }
        break;
    case 'a': {
            MenuControl::deleteMenu ( "TestMenu" );
            mgr->redraw();
        }
        break;
    case 'w': {
            MenuControl::showMenu ( "TestMenu", true );
            mgr->redraw();
        }
        break;
    case 's': {
            MenuControl::showMenu ( "TestMenu", false );
            mgr->redraw();
        }
        break;
    case 'e': {
            MenuControl::showWidget ( "TestMenu","Button1", true );
            mgr->redraw();
        }
        break;
    case 'd': {
            MenuControl::showWidget ( "TestMenu","Button1", false );
            mgr->redraw();
        }
        break;
    }
}

int setupGLUT ( int *argc, char *argv[] ) {
    glutInit ( argc, argv );
    glutInitDisplayMode ( GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE );
    int winid = glutCreateWindow ( "OpenSG" );
    glutReshapeFunc ( reshape );
    glutDisplayFunc ( display );
    glutMouseFunc ( mouse );
    glutMotionFunc ( motion );
    glutKeyboardFunc ( keyboard );
    glutIdleFunc ( display );
    return winid;
}

