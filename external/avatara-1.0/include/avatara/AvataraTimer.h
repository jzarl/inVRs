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

//_______________________________________________________
//
/// \file AvataraTimer.h
/// Simple multipurpose timer class.
//_______________________________________________________

#ifndef _DR_TIMER_H
  #define _DR_TIMER_H

#include "avatara/AvataraDllExports.h"
 #include <time.h>

namespace Utilities
{

  //-------------------------------------------------------
  /// Simple multipurpose timer.
  //-------------------------------------------------------
  class AVATARA_API Timer
  {
  public:
    Timer(void);
    ~Timer(void);
    void Start(void);
    void Stop(void);
    void Reset(void);
    float GetTime(void) const;
    void SetTime(float t);
    int GetTimeInMilliseconds(void) const;
    void SetTimeInMilliseconds(int t);
    float GetSpeed() const;
    void SetSpeed(float s);

  private:
    void SetSysStartTime();
    float ReadClock() const;

  private:
    time_t sysStartTime; ///< Time [s] when timer was initialized.
    float startTime;    ///< Time [s] when timer was started.
    float time;         ///< Timer-value [s]
    bool isRunning;     ///< Flag, showing that timer is active.
    float speedFactor;  ///< Speed factor accelerate or slow down timer (default: 1.0)
  };

}

#endif // _DR_TIMER_H
