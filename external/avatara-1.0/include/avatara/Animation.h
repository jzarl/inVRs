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
 * rlander(rlander@inVRs.org):                                               *
 *  Added statements for dll-support under windows                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

//_____________________________________________________________
//
/// \file Animation.h
/// Animation and AnimationSet definition.
//_____________________________________________________________
//

#ifndef _DR_ANIMATION_H
  #define _DR_ANIMATION_H

#include "avatara/Skeleton.h"
#include "avatara/Pose.h"
#include <map>

namespace Avatara
{

  //----------------------------------------------------------------
  /// Contains all poses of one animation.
  /// Class Animation handles all functions for one animation cycle.
  //----------------------------------------------------------------
  class AVATARA_API Animation
  {
  public:
    Animation();
    ~Animation();

    bool Load(std::string fileName);
    const Pose& GetPose(int key) const;
    const Pose& GetPrecPose(int time) const;
    const Pose& GetSuccPose(int time) const;
    int GetAnimationLength() const;

  private:
    Animation(const Animation&);              // forbid use of copy constructor
    Animation& operator=(const Animation&);   // forbid use of assignment operator

    void UnLoad();

    int version;              ///< Version of animation file format.
    int noOfBones;            ///< Number of bones in skeleton.
    int noOfPoses;            ///< Number of boses in animation.
    int animationLength;      ///< length of animation cycle in ms.
    Pose* pPoses;             ///< Array of keyframe poses.
  };


  //----------------------------------------------------------------
  /// Contains a map of animations.
  /// Class AnimationSet is a container for animations. Animations
  /// can be accessed by an animation name (string).
  //----------------------------------------------------------------
  class AVATARA_API AnimationSet
  {
  public:
    AnimationSet();
    ~AnimationSet();
    void Add(std::string name, const Animation* pAnimation);
    const Animation* GetAnimation(std::string name) const;
    const Animation* GetAnimation(int index) const;

  private:
    AnimationSet(const AnimationSet&);            // forbid use of copy constructor
    AnimationSet& operator=(const AnimationSet&); // forbid use of assignment operator

    /// One element of the animations map: (name, animation).
    typedef std::pair<std::string, const Animation*> AnimationPair;

    /// Map with animations, ordered by name.
    std::map <std::string, const Animation*> animations;
  };
}

#endif
