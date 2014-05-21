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

//_____________________________________________________________
//
/// \file Animation.cpp
/// Animation and AnimationSet implementation.
//_____________________________________________________________

#include "avatara/Animation.h"
#include "avatara/Utilities.h"
#include "avatara/Debug.h"
#include "avatara/Quaternion.h"
#include <iostream>
#include <cstring>
#include <cstdio>

using namespace Algebra;
using namespace std;
using namespace Utilities;


namespace Avatara
{

  //-------------------------------------------------------
  /// Initializes empty animation object.
  //-------------------------------------------------------
  Animation::Animation()
  {
    version = 0;
    noOfBones = 0;
    noOfPoses = 0;
    animationLength = 0;
    pPoses = 0;
  }


  //-------------------------------------------------------
  /// Performes clean-up.
  //-------------------------------------------------------
  Animation::~Animation()
  {
    UnLoad();
  }


  //-------------------------------------------------------
  /// Loads animation from file.
  ///
  /// \param fileName Path of animation file.
  /// \return True if success, otherwise error.
  //-------------------------------------------------------
  bool Animation::Load(string fileName)
  {
    PRINT("Animation::Load(\"" + fileName + "\")");

    ASSERT(!fileName.empty());

    int i = 0;
    int intVar = 0;                               // buffer variable
    float floatVar = 0.f;                         // buffer variable
    char pBuffer[1024]; memset(pBuffer, 0, 1024); // bigger buffer

    UnLoad();

    //
    // open file
    //
    FILE* file = fopen(fileName.c_str(), "rb");
    if (file == 0)
      return false;

    //
    // read file tag "AVATARA\0"
    //
    fread(pBuffer, sizeof(char), 8, file);
    if(strcmp(pBuffer, "AVATARA") != 0)
    {
      cerr << "File is no Avatara File!" << endl;
      fclose(file);
      return false;
    }

    //
    // read file type "ANIMATION\0"
    //
    fread(pBuffer, sizeof(char), 12, file);
    if(strcmp(pBuffer, "ANIMATION") != 0)
    {
      cerr << "File is no Avatara Animation File!" << endl;
      fclose(file);
      return false;
    }

    //
    // read version
    //
    fread(&intVar, sizeof(int), 1, file);
    version = ConvertLittleEndianToNative(intVar);

    //
    // read number of bones and poses
    //
    fread(&intVar, sizeof(int), 1, file);
    noOfBones = ConvertLittleEndianToNative(intVar);
    DEBUG(noOfBones);
    ASSERT(noOfBones > 0);
    fread(&intVar, sizeof(int), 1, file);
    noOfPoses = ConvertLittleEndianToNative(intVar);
    DEBUG(noOfPoses);
    ASSERT(noOfPoses > 0);

    //
    // read over reserved ints
    //
    fread(&pBuffer, sizeof(int), 2, file);

    //
    // read Pose List
    //
    pPoses = new Pose[noOfPoses];
    for(i=0; i<noOfPoses; i++)
    {
      float w, x, y, z;
      Matrix* boneMatrices = new Matrix[noOfBones];


      //
      // read time of key frame
      //
      fread(&intVar, sizeof(int), 1, file);
      pPoses[i].time = ConvertLittleEndianToNative(intVar);

      // allocate memory for bone keys in pose object
      pPoses[i].AllocateBoneKeys(noOfBones);

      //
      // read bone indexes and weights
      //
      for(int j=0; j<noOfBones; j++)
      {
        //
        // read Quaternion
        //
        fread(&floatVar, sizeof(float), 1, file);
        w = ConvertLittleEndianToNative(floatVar);
        fread(&floatVar, sizeof(float), 1, file);
        x = ConvertLittleEndianToNative(floatVar);
        fread(&floatVar, sizeof(float), 1, file);
        y = ConvertLittleEndianToNative(floatVar);
        fread(&floatVar, sizeof(float), 1, file);
        z = ConvertLittleEndianToNative(floatVar);
        Quaternion quat(w, x, y, z);

        //
        // read location
        //
        fread(&floatVar, sizeof(float), 1, file);
        x = ConvertLittleEndianToNative(floatVar);
        fread(&floatVar, sizeof(float), 1, file);
        y = ConvertLittleEndianToNative(floatVar);
        fread(&floatVar, sizeof(float), 1, file);
        z = ConvertLittleEndianToNative(floatVar);
        Vector loc(x, y, z);

        //
        // read scale
        //
        fread(&floatVar, sizeof(float), 1, file);
        x = ConvertLittleEndianToNative(floatVar);
        fread(&floatVar, sizeof(float), 1, file);
        y = ConvertLittleEndianToNative(floatVar);
        fread(&floatVar, sizeof(float), 1, file);
        z = ConvertLittleEndianToNative(floatVar);
        Vector scale(x, y, z);

        //
        // store as bone key
        //
        BoneKey boneKey(quat, loc, scale);
        pPoses[i].SetBoneKey(j, boneKey);
      }

      delete[] boneMatrices;
    }

    // store total animation length (equal to time of last pose)
    animationLength = pPoses[noOfPoses-1].time;

    // close file
    fclose(file);

    PRINT("Animation::Load() finished")

    return true;
  }


  //-------------------------------------------------------
  /// Returns pose of key frame.
  ///
  /// \param key  Index of key frame.
  /// \return Pose of key frame.
  //-------------------------------------------------------
  const Pose& Animation::GetPose(int key) const
  {
    ASSERT(key < noOfPoses);
    ASSERT(pPoses != 0);
    return pPoses[key];
  }


  //-------------------------------------------------------
  /// Returns preceding key frame for specified time.
  ///
  /// \param time  Animation time in ms.
  /// \return Pose of preceding key frame.
  //-------------------------------------------------------
  const Pose& Animation::GetPrecPose(int time) const
  {
    ASSERT(noOfPoses > 0);
    ASSERT(pPoses != 0);

    if (time <= 0)
      return pPoses[0];      // return first key frame for negative time
    else
    {
      int i = 0;

      while (i<noOfPoses && pPoses[i].time <= time)
        i++;
      i--; // Get predecessor
      return pPoses[i];
    }
  }


  //----------------------------------------------------------------
  /// Returns succeeding key frame for specified time.
  /// Animation is considered as cyclic for times > animationLength.
  ///
  /// \param time  Animation time in ms.
  /// \return Pose of succeeding key frame.
  //----------------------------------------------------------------
  const Pose& Animation::GetSuccPose(int time) const
  {
	ASSERT(noOfPoses > 0);
    ASSERT(pPoses != 0);

    if (noOfPoses == 1)
      return pPoses[0];

    int i = 0;
    while (i<noOfPoses && pPoses[i].time < time)
      i++;
    if (i >= noOfPoses)
    {
      // Animations are cyclic --> last pose is actually first pose
      i = 1;
    }
    return pPoses[i];
  }


  //-------------------------------------------------------
  /// Returns length of animation in ms.
  ///
  /// \return Total length of animation in ms.
  //-------------------------------------------------------
  int Animation::GetAnimationLength() const
  {
    return animationLength;
  }


  //-------------------------------------------------------
  /// Cleans up resources.
  //-------------------------------------------------------
  void Animation::UnLoad()
  {
    //
    // free Pose list
    //
    if (pPoses != 0)
    {
      delete [] pPoses;
      pPoses = 0;
    }
  }


  //--------------------------------------------------------------------------


  //-------------------------------------------------------
  /// Initializes animation set.
  //-------------------------------------------------------
  AnimationSet::AnimationSet()
  {
  }


  //-------------------------------------------------------
  /// Cleans up resources.
  //-------------------------------------------------------
  AnimationSet::~AnimationSet()
  {
  }


  //-------------------------------------------------------
  /// Adds animation to animation map.
  ///
  /// \param name Name of animation (e.g. "walk").
  /// \param pAnimation Pointer to animation object.
  //-------------------------------------------------------
  void AnimationSet::Add(string name, const Animation* pAnimation)
  {
    ASSERT(!name.empty());
    ASSERT(pAnimation != 0);
    animations.insert(AnimationPair(name, pAnimation));
  }


  //-------------------------------------------------------
  /// Returns animation for specified name.
  ///
  /// \param name Name of animation (e.g. "walk").
  /// \return Pointer to animation object. 0 if no animation
  /// was found.
  //-------------------------------------------------------
  const Animation* AnimationSet::GetAnimation(string name) const
  {
    map<string, const Animation*>::const_iterator iter;
    iter = animations.find(name);
    if (iter == animations.end())
    {
      PRINT("Animation \"" + name + "\" not found in AnimationSet.")
      return 0;
    }
    else
      return iter->second;
  }

  //-------------------------------------------------------
  /// Returns animation for specified name.
  ///
  /// \param index Index of animation.
  /// \return Pointer to animation object. 0 if no animation
  /// was found.
  //-------------------------------------------------------
  const Animation* AnimationSet::GetAnimation(int index) const
  {
    ASSERT(index<(int)animations.size());
    map<string, const Animation*>::const_iterator iter;
    iter = animations.begin();
    for (int i=0; i<index; i++)
      iter++;
    return iter->second;
  }
}
