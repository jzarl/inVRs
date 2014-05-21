/*---------------------------------------------------------------------------*\
 *           interactive networked Virtual Reality system (inVRs)            *
 *                                                                           *
 *    Copyright (C) 2005-2009 by the Johannes Kepler University, Linz        *
 *                                                                           *
 *                            www.inVRs.org                                  *
 *                                                                           *
 *              contact: canthes@inVRs.org, rlander@inVRs.org                *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                License                                    *
 *                                                                           *
 * This library is free software; you can redistribute it and/or modify it   *
 * under the terms of the GNU Library General Public License as published    *
 * by the Free Software Foundation, version 2.                               *
 *                                                                           *
 * This library is distributed in the hope that it will be useful, but       *
 * WITHOUT ANY WARRANTY; without even the implied warranty of                *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         *
 * Library General Public License for more details.                          *
 *                                                                           *
 * You should have received a copy of the GNU Library General Public         *
 * License along with this library; if not, write to the Free Software       *
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                 *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                        Project: CAVESceneManager                          *
 *                                                                           *
 * The CAVE Scene Manager (CSM) is a wrapper for the OpenSG multi-display    *
 * functionality, which allows the easy configuration of a multi-display     *
 * setup via text files. It was developed by Adrian Haffegee of the Centre   *
 * for Advanced Computing and Emerging Technologies (ACET) at the            *
 * University of Reading, UK as part of his MSc Thesis.                      *
 *                                                                           *
\*---------------------------------------------------------------------------*/

// OpenSG CSM render server
//
// Mono OpenSG cluster server. In OpenSG
// the terms server and client are used similar to X11. The
// application is the client. Instances that are used for
// rendering are called server.
//
// See the ClusterClient.cpp for an example of how to use them.
//   ./server-mono -g 300x300+500+100 -a origin -w front &
//   ./server-stereo -g 300x300+500+100 -a origin -w front &
//   ./server-stereo -display :0.0 -a exodus front &

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#ifdef WIN32
#include <Ws2tcpip.h> //must be included before windows.h
#else
#include <arpa/inet.h>
#endif

#include "../include/OSGCSM/compatibility.h"

// General OpenSG configuration, needed everywhere
#include <OpenSG/OSGConfig.h>
// GLUT is used for window handling
#include <OpenSG/OSGGLUT.h>
// The Cluster server definition
#include <OpenSG/OSGClusterServer.h>
// The GLUT-OpenSG connection class
#include <OpenSG/OSGGLUTWindow.h>
// Render action definition.
#include <OpenSG/OSGRenderAction.h>
#include <OpenSG/OSGSocketException.h>


OSG_USING_NAMESPACE

OSGCompat::pointer_traits<OSG::GLUTWindow>::RecPtr window;
OSGCompat::pointer_traits<OSG::RenderAction>::OsnmPtr ract;

// pointer the the cluster server instance
ClusterServer  *server = 0;

// forward declarations
void display();
void reshape( int width, int height);

struct WindowGeometry
{
	int x, y, w, h;
};

void usage(const std::string& applicationName)
{
	std::cout   << "Usage: "                             << " "
				<< applicationName                       << " "
				<< "-m (multicast)"                      << " " 
				<< "-w (windowed)"                       << " " 
				<< "-a <ip address of head node>"          << " " 
				<< "-g (WindowGeometry) <w>x<h>+<x>+<y>" << " " 
				<< "-t (always on top)"                  << " " 
				<< "-s (stereo)"                         << " " 
				<< endLog;
}

bool parseWindowGeometry(const std::string& geometryStr, WindowGeometry& geometry)
{
	if (const char* charXPos = std::strchr(geometryStr.c_str(), 'x'))
	{		
		int w = std::atoi(geometryStr.c_str());
		int h = std::atoi(charXPos+1);

		if ((w > 0) && (h > 0))
		{
			if (const char* char1stPlusPos = std::strchr(charXPos, '+')) 
			{
				int x = std::atoi(char1stPlusPos+1);
				if (const char* char2ndPlusPos = std::strchr(char1stPlusPos, '+')) 
				{
					int y = std::atoi(char2ndPlusPos+1);
					geometry.x = x;
					geometry.y = y;
					geometry.w = w;
					geometry.h = h;
					return true;
				}
			}
		}
	}
	return false;
}


/// Checks whether the parameter string consists of a valid IP4 address
/// Requires WinSocket2 library in Windows (ws2_32), not available in Windows XP and below
bool validateAddress(const std::string& address)
{
	unsigned long dummy = 0;
	return 0 != inet_pton(AF_INET, address.c_str(), &dummy);
}

bool parseCmdLineArgs(int argc, char** argv, 
	std::string& name, std::string& connectionType, std::string& address, 
	WindowGeometry& geometry,
	bool& fullscreen, bool& always_on_top, bool& stereo)
{
    for(int i=1 ; i<argc ; ++i)
    {
        if(argv[i][0] == '-')
        {
			std::string applicationName(argv[0]);
			std::string geometryStr;
			int nextArgIdx;
			
            switch(argv[i][1])
            {
    		case 's':
				stereo = true;
				break;
            case 'm':
                connectionType="Multicast";
                break;
            case 'w':
                fullscreen=false;
                break;
			case 't':
				always_on_top = true;
				break;
            case 'a':
				nextArgIdx = ++i;
				if (nextArgIdx >= argc)
				{
					std::cerr << "ERROR: No address specified" << std::endl;
					usage(applicationName);
					return false;
				}
                address = argv[nextArgIdx];
				if (!validateAddress(address))
				{
					std::cerr << "ERROR: No valid IP4 address (in the form xxx.xxx.xxx.xxx) specified" << std::endl;
					usage(applicationName);
					return false;
				}
                break;
            case 'g':
				nextArgIdx = ++i;
				if (nextArgIdx >= argc)
				{
					std::cerr << "ERROR: No Window Geometry specified" << std::endl;
					usage(applicationName);
					return false;
				}
				geometryStr = argv[nextArgIdx];
				if (!parseWindowGeometry(geometryStr, geometry))
				{
					std::cerr << "ERROR: Malformed geometry: " << geometryStr << std::endl;
					usage(applicationName);
					return false;
				}
                break;
            case 'd':
				nextArgIdx = ++i;
                //display will be caught by glutInit, skip it, + its value
                /*if (strcmp(&argv[i][1], "display")==0) 
				{
                }*/
				break;
                // incorrect value falls through to bomb out...
            default:
                usage(applicationName);
                return false;
            }
        }
        else
        {
            name = argv[i];
        }
    }
	return true;
}

void printConfiguration(const std::string& name, 
	const std::string& address, 
	const std::string& connectionType, 
	bool stereo, 
	bool fullscreen, 
	bool always_on_top)
{
	std::cout << "Server configuration: " 
		<< "Name: "       << name                      << ", "
		<< "Address: "    << address                   << ", "
		<< "Connection: " << connectionType            << ", "
		<< "Display: " << (stereo ? "stereo" : "mono") << ", "
		<< (fullscreen ? "fullscreen" : "windowed")    
		<< (always_on_top ? ", always on top" : "")    << std::endl;
}


void setAlwaysOnTop(const std::string& windowName)
{
 // always on top hack
#ifdef WIN32
			HWND hwnd = FindWindow( "GLUT", windowName.c_str() ); //get its handle "GLUT" = class name = window caption
			SetWindowPos( hwnd, HWND_TOPMOST, NULL, NULL, NULL, NULL, SWP_NOREPOSITION | SWP_NOSIZE ); //set the window always-on-top
#else
			std::cout << "always on top not yet implemented on this platform\n";
#endif
}

// Initialize GLUT & OpenSG and start the cluster server
int main(int argc,char **argv)
{
#ifdef WIN32
	OSG::preloadSharedObject("OSGFileIO");
    OSG::preloadSharedObject("OSGImageFileIO");
	OSG::preloadSharedObject("OSGEffectGroups");
#endif
	ChangeList::setReadWriteDefault();
    osgInit(argc, argv);
	
	std::string name("ClusterServer");
    std::string connectionType("StreamSock");
    std::string address("127.0.0.1");
	bool fullscreen = true;
	bool always_on_top = false;
	bool stereo = false;
	WindowGeometry geometry = {0,0,500,500};

	if (!parseCmdLineArgs(argc, argv, name, connectionType, address, geometry, fullscreen, always_on_top, stereo))
		return 0;

	printConfiguration(name, address, connectionType, stereo, fullscreen, always_on_top);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | (stereo ? GLUT_STEREO : 0));
	glutInitWindowPosition(geometry.x, geometry.y);
	glutInitWindowSize(geometry.w, geometry.h);
    int winid = glutCreateWindow(name.c_str());
    if(fullscreen) 
	{
        glutFullScreen();
    }
	if (always_on_top)
	{
		setAlwaysOnTop(name);
	}
    glutDisplayFunc(display);
    glutIdleFunc(display);
    glutReshapeFunc(reshape);
    glutSetCursor(GLUT_CURSOR_NONE);

    ract = RenderAction::create();

    window     = GLUTWindow::create();
	OSGCompat::setGlutId(window, winid);
    window->init();

	bool failed = false;
	do
	{
		try 
		{
			delete server;
			server = new ClusterServer(window, name, connectionType, address);
			server->start();
			failed = false;
		}
		catch (OSG_STDEXCEPTION_NAMESPACE::exception& e)
		{
			SLOG << "ERROR: " << e.what() << endLog;
			SLOG << "Attempting to restart ClusterServer..." << endLog;
			failed = true;
		}
	} while (failed);

    glutMainLoop();

    return 0;
}
static bool disconnected = false;
void display()
{
    try
    {
		if (!disconnected)
		{
			// receive scenegraph and do rendering
	        server->render(ract);
			OSGCompat::clearChangelist();
		}
		else
		{
			SLOG << "Attempting to restart ClusterServer..." << endLog;
			// start server, wait for client to connect
			server->start();
			glutShowWindow();
			disconnected = false;
		}
    }
    catch(std::exception &e)
    {
        SLOG << "ERROR: " << e.what() << endLog;
		disconnected = true;
		// try to restart server
        server->stop();
		//after glutHideWindow() control has to be given back to the glutloop for it to have an effect
		glutHideWindow();
	}
}

void reshape( int width, int height )
{
	window->resize( width, height );
}
