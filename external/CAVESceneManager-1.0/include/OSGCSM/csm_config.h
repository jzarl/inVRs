#ifndef CAVESCENEMANAGER_INCLUDE_OPENSG_CSM_CONFIG_H_43908043
#define CAVESCENEMANAGER_INCLUDE_OPENSG_CSM_CONFIG_H_43908043

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

//@RLANDER: added definitions for windows-dll compatibility
// API macros
#if defined(WIN32)
	#ifdef CAVESCENEMANAGER_COMPILELIB
		#define CAVESCENEMANAGER_DLLMAPPING __declspec(dllexport)
	#else
		#define CAVESCENEMANAGER_DLLMAPPING __declspec(dllimport)
	#endif
#else
	#define CAVESCENEMANAGER_DLLMAPPING
#endif

#endif
