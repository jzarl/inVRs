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

//_______________________________________________________
//
/// \file Utilities.cpp
/// Miscellaneous helper functions.
//_____________________________________________________________

#include "avatara/Utilities.h"
#include "avatara/Debug.h"
#include <algorithm>

using namespace std;


//-------------------------------------------------------
/// Auxiliary classes and operations.
//-------------------------------------------------------
namespace Utilities
{

  //---------------------------------------------------------
  /// Converts long value to hexadecimal string.
  /// \param value long value.
  //---------------------------------------------------------
  string ToHexString(long value)
  {
    ostringstream stream;
    stream.unsetf(ios_base::dec);
    stream.setf(ios_base::hex);
    stream << value << ends;
    return stream.str();
  }


  //----------------------------------------------------------
  /// Converts 4-byte buffer to native format.
  ///
  /// \param pBytes Array with 4 bytes in little endian order.
  /// \return 4-byte array in native format.
  //----------------------------------------------------------
  void ConvertLittleEndianToNative(unsigned char* pBytes)
  {
    #ifdef BIGENDIAN
      PRINT("ConvertLittleEndianToNative not tested!")
      swap(pBytes[3], pBytes[0]);
      swap(pBytes[2], pBytes[1]);
    #else
      /* do nothing */
      ;
    #endif
  }


  //-------------------------------------------------------
  /// Converts integer to native format.
  ///
  /// \param i Integer in little endian format.
  /// \return Integer in native format.
  //-------------------------------------------------------
  int ConvertLittleEndianToNative(int i)
  {
    ASSERT(sizeof(int) == 4);
    ConvertLittleEndianToNative((unsigned char*) &i);
    return i;
  }


  //-------------------------------------------------------
  /// Converts float to native format.
  ///
  /// \param f float in little endian format.
  /// \return float in native format.
  //-------------------------------------------------------
  float ConvertLittleEndianToNative(float f)
  {
    ASSERT(sizeof(float) == 4);
    ConvertLittleEndianToNative((unsigned char*) &f);
    return f;
  }

}
