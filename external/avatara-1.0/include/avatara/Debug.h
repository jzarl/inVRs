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

#ifndef _DR_DEBUG_H
  #define _DR_DEBUG_H

#include <iostream>
#include <assert.h>


#if defined(_DEBUG)

  /// Prints variable name and content if _DEBUG is defined.
  /// \param x  variable
  #define DEBUG(x) std::cerr <<  #x " = " << (x) << std::endl;

  /// Prints expression if _DEBUG is defined and evaluates it (always!).
  /// \param e  expression
  #define TRACE(e) std::cerr << #e << std::endl; (e);

  /// Prints message if _DEBUG is defined.
  /// \param m  message
  #define PRINT(m) std::cerr << (m) << std::endl;

  /// Aborts program if condition is false if _DEBUG is defined.
  /// \param c  condition
  #define ASSERT(c) assert(c);

#else

  /// Prints variable name and content if _DEBUG is defined.
  /// \param x  variable
  #define DEBUG(x)

  /// Prints expression if _DEBUG is defined and evaluates it (always!).
  /// \param e  expression
  #define TRACE(e) (e);

  /// Prints message if _DEBUG is defined.
  /// \param m  message
  #define PRINT(m)

  /// Aborts program if condition is false if _DEBUG is defined.
  /// \param c  condition
  #define ASSERT(c)

#endif

#endif // _DR_DEBUG_H
