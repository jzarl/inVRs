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

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

/*! \file appctrl.h
 *
 *  Adrian Haffegee, June 2005
 *  header for the application control utilities
 */

#ifndef _CVR_APPCTRL_H_
#define _CVR_APPCTRL_H_

#include "compatibility.h"
#include "OSGCAVEConfig.h"
#include <OpenSG/OSGMultiDisplayWindow.h>

namespace OSGCSM
{
	CAVESCENEMANAGER_DLLMAPPING int startServers(const CAVEConfig& cfg, const char* serverAdress = "localhost", const char* serverPath=0);

	CAVESCENEMANAGER_DLLMAPPING OSGCompat::pointer_traits<OSG::MultiDisplayWindow>::TransitPtr createAppWindow(const CAVEConfig& cfg, const std::string& broadcastAddress = "");

	#ifdef WIN32
	CAVESCENEMANAGER_DLLMAPPING PROCESS_INFORMATION* csmServerHandles;
	#endif
}

#endif // _CVR_APPCTRL_H_
