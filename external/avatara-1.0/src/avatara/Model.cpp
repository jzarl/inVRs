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
/// \file Model.cpp
/// Model implementation.
//_____________________________________________________________

#include "avatara/Model.h"
#include "avatara/Debug.h"
#include "avatara/Utilities.h"
#include <iostream>
#include <cstdio>
#include <memory.h>

using namespace std;
using namespace Algebra;
using namespace Utilities;


namespace Avatara
{
  //-------------------------------------------------------
  /// Initializes empty model object.
  //-------------------------------------------------------
  Model::Model()
  {
    version = 0;
    noOfVertices = 0;
    pVertices = 0;
    noOfFaces = 0;
    pFaces = 0;
  }

  //-------------------------------------------------------
  /// Frees resources.
  //-------------------------------------------------------
  Model::~Model()
  {
    UnLoad();
  }


  //-------------------------------------------------------
  /// Loads model from file.
  ///
  /// \param fileName Path of model file.
  /// \return True if success, otherwise false.
  //-------------------------------------------------------
  bool Model::Load(string fileName)
  {
    PRINT("Model::Load(\"" + fileName + "\")");

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
    // read file type "MODEL\0"
    //
    fread(pBuffer, sizeof(char), 12, file);
    if(strcmp(pBuffer, "MODEL") != 0)
    {
      cerr << "File is no Avatara Model File!" << endl;
      fclose(file);
      return false;
    }

    //
    // read version
    //
    fread(&intVar, sizeof(int), 1, file);
    version = ConvertLittleEndianToNative(intVar);
    DEBUG(version);

    //
    // read number of vertices, faces and bones
    //
    fread(&intVar, sizeof(int), 1, file);
    noOfVertices = ConvertLittleEndianToNative(intVar);
    DEBUG(noOfVertices);
    ASSERT(noOfVertices > 0);

    fread(&intVar, sizeof(int), 1, file);
    noOfFaces = ConvertLittleEndianToNative(intVar);
    DEBUG(noOfFaces);
    ASSERT(noOfFaces > 0);

    int noOfBones = 0;
    fread(&intVar, sizeof(int), 1, file);
    noOfBones = ConvertLittleEndianToNative(intVar);
    DEBUG(noOfBones);
    ASSERT(noOfBones > 0);

    //
    // read over reserved ints
    //
    fread(&pBuffer, sizeof(int), 3, file);

    //
    // read Vertex List
    //
    pVertices = new Vertex[noOfVertices];
    for(i=0; i<noOfVertices; i++)
    {
      float x, y, z;

      //
      // read vertex coordinate
      //
      fread(&floatVar, sizeof(float), 1, file);
      x = ConvertLittleEndianToNative(floatVar);
      fread(&floatVar, sizeof(float), 1, file);
      y = ConvertLittleEndianToNative(floatVar);
      fread(&floatVar, sizeof(float), 1, file);
      z = ConvertLittleEndianToNative(floatVar);
      pVertices[i].Position().Set(x, y, z);

      //
      // read normal
      //
      fread(&floatVar, sizeof(float), 1, file);
      x = ConvertLittleEndianToNative(floatVar);
      fread(&floatVar, sizeof(float), 1, file);
      y = ConvertLittleEndianToNative(floatVar);
      fread(&floatVar, sizeof(float), 1, file);
      z = ConvertLittleEndianToNative(floatVar);
      pVertices[i].Normal().Set(x, y, z);

      if(noOfBones > 0)
      {
        // ----- read bone info

        //
        // read no of affecting bones
        //
        fread(&intVar, sizeof(int), 1, file);
        int noOfAffectingBones = ConvertLittleEndianToNative(intVar);

        if(noOfAffectingBones > 0)
        {
          pVertices[i].AllocateBoneList(noOfAffectingBones);

          //
          // read bone indexes and weights
          //
          for(int j=0; j<pVertices[i].NoOfBones(); j++)
          {
            fread(&intVar, sizeof(int), 1, file);
            pVertices[i].SetBoneIndex(j, ConvertLittleEndianToNative(intVar));
            fread(&floatVar, sizeof(float), 1, file);
            pVertices[i].SetBoneWeight(j, ConvertLittleEndianToNative(floatVar));
          }
        }
      }
    }

    //
    // read face list
    //
    pFaces = new Face[noOfFaces];
    for(i=0; i<noOfFaces; i++)
    {
      int j=0;

      //
      // read vertex indices
      //
      fread(&intVar, sizeof(int), 1, file);
      pFaces[i].pVertexIndices[0] = ConvertLittleEndianToNative(intVar);
      fread(&intVar, sizeof(int), 1, file);
      pFaces[i].pVertexIndices[1] = ConvertLittleEndianToNative(intVar);
      fread(&intVar, sizeof(int), 1, file);
      pFaces[i].pVertexIndices[2] = ConvertLittleEndianToNative(intVar);

      //
      // read shading style
      //
      fread(&intVar, sizeof(int), 1, file);
      pFaces[i].shading = (ShadingStyle) ConvertLittleEndianToNative(intVar);

      //
      // read vertex coordinate
      //
      float x, y, z;
      fread(&floatVar, sizeof(float), 1, file);
      x = ConvertLittleEndianToNative(floatVar);
      fread(&floatVar, sizeof(float), 1, file);
      y = ConvertLittleEndianToNative(floatVar);
      fread(&floatVar, sizeof(float), 1, file);
      z = ConvertLittleEndianToNative(floatVar);
      pFaces[i].normal.Set(x, y, z);

      //
      // read vertex colors
      //
      for(j=0; j<3; j++)
      {
        fread(&(pFaces[i].pColors[j].r), sizeof(unsigned char), 1, file);
        fread(&(pFaces[i].pColors[j].g), sizeof(unsigned char), 1, file);
        fread(&(pFaces[i].pColors[j].b), sizeof(unsigned char), 1, file);
        fread(&(pFaces[i].pColors[j].a), sizeof(unsigned char), 1, file);
      }

      //
      // read vertex uv coordinates
      //
      for(j=0; j<3; j++)
      {
        fread(&floatVar, sizeof(float), 1, file);
        pFaces[i].pTextureCoords[j].u = ConvertLittleEndianToNative(floatVar);
        fread(&floatVar, sizeof(float), 1, file);
        pFaces[i].pTextureCoords[j].v = ConvertLittleEndianToNative(floatVar);
      }
    }

    bool bSuccessful = LoadBones(file, noOfBones);

    // close file
    fclose(file);

    PRINT("Model::Load() finished")

    return bSuccessful;
  }


  //-------------------------------------------------------
  /// Loads bones.
  //-------------------------------------------------------
  bool Model::LoadBones(FILE* file, int noOfBones)
  {
    if (noOfBones > 0)
    {
      Bone* pBones = new Bone[noOfBones];
      int intVar = 0;
      float floatVar = 0.0f;
      char pBuffer[1024]; memset(pBuffer, 0, 1024); // bigger buffer

      for(int i=0; i<noOfBones; i++)
      {
        //
        // read parent id
        //
        fread(&intVar, sizeof(int), 1, file);
        pBones[i].parentID = ConvertLittleEndianToNative(intVar);
        DEBUG(pBones[i].parentID)

        //
        // read bone name
        //
        fread(&pBuffer, sizeof(char), 16, file);
        pBones[i].name = pBuffer;
        DEBUG(pBones[i].name)


        if (version == 1)
        {
          // Blender type of bones (Head, Tail, Roll)
          float x, y, z;
          Vector head;
          Vector tail;
          float roll = 0.0f;


          //
          // read head
          //
          fread(&floatVar, sizeof(float), 1, file);
          x = ConvertLittleEndianToNative(floatVar);
          fread(&floatVar, sizeof(float), 1, file);
          y = ConvertLittleEndianToNative(floatVar);
          fread(&floatVar, sizeof(float), 1, file);
          z = ConvertLittleEndianToNative(floatVar);
          head.Set(x, y, z);

          //
          // read tail
          //
          fread(&floatVar, sizeof(float), 1, file);
          x = ConvertLittleEndianToNative(floatVar);
          fread(&floatVar, sizeof(float), 1, file);
          y = ConvertLittleEndianToNative(floatVar);
          fread(&floatVar, sizeof(float), 1, file);
          z = ConvertLittleEndianToNative(floatVar);
          tail.Set(x, y, z);

          //
          // read roll
          //
          fread(&floatVar, sizeof(float), 1, file);
          roll = ConvertLittleEndianToNative(floatVar);

          Bone* pParent = (pBones[i].parentID == -1) ? 0 : &pBones[pBones[i].parentID];
          pBones[i].boneToWorldMatrix = ComputeBoneToWorldMatrix(head, tail, roll, pParent);
          pBones[i].length = (tail - head).Length();
        }
        else if (version == 2)
        {
          // 3dsmax type of bones (Transform)
          for (int column = 0; column < 4; column++)
          {
            for (int row = 0; row < 3; row++)
            {
              fread(&floatVar, sizeof(float), 1, file);
              floatVar = ConvertLittleEndianToNative(floatVar);
              pBones[i].boneToWorldMatrix.M(row, column) = floatVar;
            }
            pBones[i].boneToWorldMatrix.M(3, column) = (column == 3) ? 1.0f : 0.0f;
          }

          fread(&floatVar, sizeof(float), 1, file);
          floatVar = ConvertLittleEndianToNative(floatVar);
          pBones[i].length = floatVar;

        }
        else
        {
          return false;
        }
      }

      skeleton.SetNoOfBones(noOfBones);
      skeleton.SetBoneArray(pBones);
      skeleton.CalculateMatrices();
    }
    return true;
  }


  //-------------------------------------------------------
  /// Frees resources.
  //-------------------------------------------------------
  void Model::UnLoad()
  {
    //
    // free vertex list
    //
    if (pVertices != 0)
    {
      delete [] pVertices;
      pVertices = 0;
    }

    //
    // free face list
    //
    if (pFaces != 0)
    {
      delete [] pFaces;
      pFaces = 0;
    }

    skeleton.UnLoad();
  }


  //-------------------------------------------------------
  /// Returns number of vertices.
  //-------------------------------------------------------
  int Model::GetNoOfVertices() const
  {
    return noOfVertices;
  }


  //-------------------------------------------------------
  /// Returns vertex with specified index.
  //-------------------------------------------------------
  const Vertex& Model::GetVertex(int index) const
  {
    ASSERT(index < noOfVertices);
    ASSERT(pVertices != 0);
    return pVertices[index];
  }

  //-------------------------------------------------------
  /// Returns number of faces.
  //-------------------------------------------------------
  int Model::GetNoOfFaces() const
  {
    return noOfFaces;
  }


  //-------------------------------------------------------
  /// Returns face with specified index.
  //-------------------------------------------------------
  const Face& Model::GetFace(int index) const
  {
    ASSERT(index < noOfFaces);
    ASSERT(pFaces != 0);
    return pFaces[index];
  }


  //-------------------------------------------------------
  /// Return skeleton.
  //-------------------------------------------------------
  const Skeleton& Model::GetSkeleton() const
  {
    return skeleton;
  }


  //-------------------------------------------------------
  /// Return skeleton.
  //-------------------------------------------------------
  int Model::GetVersion() const
  {
    return version;
  }

} // namespace Avatara
