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
 *                           Project: Avatara                                *
 *                                                                           *
 * The Avatara library was developed during a practical at the Johannes      *
 * Kepler University, Linz in 2005 by Helmut Garstenauer                     *
 * (helmut@digitalrune.com) and Martin Garstenauer (martin@digitalrune.com)  *
 *                                                                           *
\*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*\
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

//_____________________________________________________________
//
/// \file OSGAvatar.h
/// Definition of Avatar for OpenSG.
//_____________________________________________________________

#ifndef _DR_OSGAVATAR_H
  #define _DR_OSGAVATAR_H

#include "avatara/Avatar.h"

#include <OpenSG/OSGGeometry.h>
#include <OpenSG/OSGImage.h>


namespace Avatara
{
  //-------------------------------------------------------
  /// A 3-dimensional avatar prepared for OpenSG.
  /// This class extends the Avatar class with OpenSG support.
  ///
  /// MakeAvatarGeo() produces a geometry object.
  /// UpdateAvatarGeo() updates the geometry to represent the
  /// actual pose (in an animation).
  //-------------------------------------------------------
  class AVATARA_API OSGAvatar : public Avatar
  {
  public:
    OSGAvatar();
    ~OSGAvatar();

#if OSG_MAJOR_VERSION >= 2
	OSG::GeometryRecPtr MakeAvatarGeo(OSG::ImageRecPtr image);
#else //OpenSG1:
	OSG::GeometryPtr MakeAvatarGeo(OSG::ImagePtr image);
#endif
    void UpdateAvatarGeo();

  private:
    OSGAvatar(const OSGAvatar&);                // forbid use of copy constructor
    OSGAvatar& operator=(const OSGAvatar&);     // forbid use of assignment operator

#if OSG_MAJOR_VERSION >= 2
    OSG::GeometryRecPtr geo;    ///<Pointer to Avatar OpenSG geometry.
#else //OpenSG1:
    OSG::GeometryPtr geo;    ///<Pointer to Avatar OpenSG geometry.
#endif
  };

}

#endif
