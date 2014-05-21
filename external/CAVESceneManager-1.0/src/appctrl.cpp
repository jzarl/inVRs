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

/*! \file appctrl.cpp
 *
 *  Adrian Haffegee, June 2005
 *  implementation for the application control utilities
 */
#include "../include/OSGCSM/compatibility.h"

#include "../include/OSGCSM/appctrl.h"

#if defined(WIN32)
#include <Windows.h>
#endif

OSG_USING_NAMESPACE

namespace OSGCSM
{



void setServerPath(char* servePthBuff, size_t buffsize, const char* serverPath, const char* appName)
{
#ifdef WIN32
	if (serverPath)
		_snprintf(servePthBuff, buffsize, "%s\\%s", serverPath, appName);
	else
		_snprintf(servePthBuff, buffsize, "%s", appName);  
#else        
	snprintf(servePthBuff, buffsize, "%s/%s", serverPath ? serverPath : ".", appName);
#endif
}

/*!
 *  returns true if any previous wall in the list has the same pipeIndex as the wall identified by currWall
 * @param cfg CAVE configuration file to use for application
 * @param currWall the index of the current wall
 * @return true if any previous wall in the list has the same pipeIndex, false otherwise
 */
bool pipeIsUsed(const CAVEConfig& cfg, int currWall)
{
	bool pipeUsed=false;
	for (int prevWall=0; prevWall < currWall; ++prevWall) 
	{
		if (cfg.getActiveWall(currWall)->getPipeIndex() ==
			cfg.getActiveWall(prevWall)->getPipeIndex()) 
		{
			pipeUsed=true;
		}
	}
	return pipeUsed;
}

/*! start server applications for rendering the display
 *
 * @param cfg CAVE configuration file to use for application
 * @return 0 if started ok, <0 if servers failed
 */
#ifdef WIN32
int startServers(const OSGCSM::CAVEConfig& cfg, const char* serverAdress, const char* serverPath)
{
	return 0;
}
#else
int startServers(const OSGCSM::CAVEConfig& cfg, const char* serverAdress, const char* serverPath)
{
	// work out path to servers
	const int BUFFSIZE = 255;
	char applicationPath[BUFFSIZE];
	const char* appName = "csmrenderserver";
	setServerPath(applicationPath, BUFFSIZE, serverPath, appName);

	fprintf(stderr, "\nOpening servers from %s\n", applicationPath);
	
	char cmdLine[2048];
	char cmdLineBuffer[2048];

#ifdef WIN32
	csmServerHandles = new PROCESS_INFORMATION[cfg.getNumActiveWalls()];
#endif
	// spawn a process for each of the servers - 1 for each active wall (that doesn't have a unique pipe)
	for (int wallIdx=0; wallIdx < cfg.getNumActiveWalls(); ++wallIdx) 
	{
		// if one of the previous walls used this pipe, continue to next wall
		if ( pipeIsUsed(cfg, wallIdx) ) 
		{
			continue;
		}

		const CAVEWall* wall = cfg.getActiveWall(wallIdx);
		std::string wallName(wall->getName());
			 
		const char* stereoStr = cfg.isStereo() ? "-s" : "";
		sprintf(cmdLineBuffer, "%s %s -g 500x500+20+20 -display %s -a %s ",
			 applicationPath, stereoStr, wall->getPipe().c_str(), serverAdress);
			 
		if (wall->isWindowed())
		{
			sprintf(cmdLine, "-w %s", wallName.c_str());
		} else {
			sprintf(cmdLine, "%s", wallName.c_str());
		}
		strcat(cmdLineBuffer, cmdLine);
		
#ifdef WIN32
		if(!strcmp(serverAdress, "127.0.0.1") || !strcmp(serverAdress, "localhost"))
		{
			fprintf(stderr, "startServers(): executing '%s'\n", cmdLineBuffer);
			STARTUPINFO si;
			ZeroMemory( &si, sizeof(si) );
			si.cb = sizeof(si);
			ZeroMemory( &csmServerHandles[wallIdx], sizeof(csmServerHandles[wallIdx]) );

			if (!CreateProcess(NULL, cmdLineBuffer, NULL, NULL, FALSE, 0, NULL, NULL, &si, &(csmServerHandles[wallIdx]))) 
			{
				fprintf(stderr, "startServers(): error at starting server! Please start server manually!\n");
				csmServerHandles[wallIdx].hProcess = INVALID_HANDLE_VALUE;
				csmServerHandles[wallIdx].hThread = INVALID_HANDLE_VALUE;
			}
			else 
			{
				assert(csmServerHandles[wallIdx].hProcess != INVALID_HANDLE_VALUE);
			} // else
//			ShellExecute(NULL, NULL, servePth, cmdLineBuffer, NULL, SW_SHOW);
		} else
		{
			fprintf(stderr, "startServers(): please start server at %s by calling %s\n",
					serverAdress, cmdLineBuffer);
		}
#else        
		switch ( fork()) 
		{
			case -1:
				perror("fork() failed");
				return -1;
				break;

			case 0:
				{
					if (!strcmp(serverAdress, "127.0.0.1") || !strcmp(serverAdress, "localhost"))
					{
						fprintf(stderr, "startServers(): executing '%s'\n", cmdLineBuffer);
						const int rc = execl(applicationPath, appName, "-g", "500x500+20+20",
								"-display", wall->getPipe().c_str(), "-a", "localhost",
							wall->isWindowed() ? "-w":"", wallName.c_str(), (void*)NULL);
						if (rc == -1)
							perror("execl failed!");
					} else {
						fprintf(stderr,
								"startServers(): please start server at %s by calling: %s\n",
								serverAdress, cmdLineBuffer);
					}
					// never get here if execl success, since control of process would have moved to exec
					exit(0);
					return -2;
					break;
				}
			default:
				fprintf(stderr, "continuing with parent process\n");
				// parent process carries on as normal
				break;
		}
#endif
	}
#ifdef WIN32
	Sleep(1000);
#else
	sleep(1);
#endif
	return 0;
}
#endif

/*!
 * Creates, configures and returns a MultiDisplayWindow instance
 * For each wall as specified by the config file, the wall's name is extracted as unique server name and added to the
 * MultiDisplayWindow instance's server list. 
 * (Uniqueness of server names: if two or more walls have the same pipe index, only the first wall's name is added to the server list)
 */
OSGCompat::pointer_traits<OSG::MultiDisplayWindow>::TransitPtr createAppWindow(const CAVEConfig& cfg, const std::string& broadcastAddress)
{
	OSGCompat::pointer_traits<OSG::MultiDisplayWindow>::RecPtr mwin = MultiDisplayWindow::create();
	
	OSGCompat::beginEditCP(mwin);
	
	if (cfg.isMCastServer()) 
	{
		mwin->setConnectionType("Multicast");
	}
	mwin->setServiceAddress(broadcastAddress);
	
	for (int wallIdx=0; wallIdx < cfg.getNumActiveWalls(); ++wallIdx) 
	{
		if ( pipeIsUsed(cfg, wallIdx) )
		{
			continue;
		}
		std::string server(cfg.getActiveWall(wallIdx)->getName());
		OSGCompat::addServer(mwin, server);
	}
	
	OSGCompat::endEditCP(mwin);
	
	return OSGCompat::pointer_traits<OSG::MultiDisplayWindow>::TransitPtr(mwin);
}

} // namespace
