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
/// \file Timer.cpp
/// Simple multipurpose timer class.
//_______________________________________________________

//___________________________________________________________________________
//
//  Includes
//___________________________________________________________________________
//
#include "avatara/AvataraTimer.h"
#include "avatara/Debug.h"
#include <sys/timeb.h>
#include <cmath>

namespace Utilities
{
  //-------------------------------------------------------
  /// Return current process time [s].
  //-------------------------------------------------------
  float Timer::ReadClock() const
  {
    struct timeb t;
    ftime(&t);
    return (float) (t.time - sysStartTime + t.millitm / 1000.0);
  }


  //-------------------------------------------------------
  /// Constructor.
  /// Initializes timer (inactive, time = 0 s).
  //-------------------------------------------------------
  Timer::Timer(void)
  : startTime(0), time(0), isRunning(false), speedFactor(1.0)
  {
    SetSysStartTime();
  }


  //-------------------------------------------------------
  /// Destructor.
  //-------------------------------------------------------
  Timer::~Timer(void)
  {
  }


  //-------------------------------------------------------
  /// Stores time [s] when timer was created.
  //-------------------------------------------------------
  void Timer::SetSysStartTime()
  {
    struct timeb t;
    ftime(&t);
    sysStartTime = t.time;
  }

  //-------------------------------------------------------
  /// Starts/resumes timer.
  //-------------------------------------------------------
  void Timer::Start(void)
  {
    if(!isRunning)
    {
      startTime = ReadClock();
      isRunning = true;
    }
  }


  //-------------------------------------------------------
  /// Stops/pauses timer.
  //-------------------------------------------------------
  void Timer::Stop(void)
  {
    if(isRunning)
    {
      time += ReadClock() - startTime;
      isRunning = false;
    }
  }


  //-------------------------------------------------------
  /// Sets timer to 0s.
  ///  This method does not stop/pause timer.
  //-------------------------------------------------------
  void Timer::Reset(void)
  {
    time = 0;
    if(isRunning)
      startTime = ReadClock();
  }


  //-------------------------------------------------------
  /// Returns current timer-value [s].
  /// Timer-value = time which has passed since timer was started.
  //-------------------------------------------------------
  float Timer::GetTime(void) const
  {
    return (isRunning) ?
      (ReadClock() - startTime + time) * speedFactor
      : time * speedFactor;
  }


  //-------------------------------------------------------
  /// Sets current timer value [s].
  /// Timer-value = time which has passed since timer was started.
  //-------------------------------------------------------
  void Timer::SetTime(float t)
  {
    time = t /  speedFactor;
    if(isRunning)
      startTime = ReadClock();
  }


  //-------------------------------------------------------
  /// Returns current timer-value [ms].
  /// Timer-value = time which has passed since timer was started.
  //-------------------------------------------------------
  int Timer::GetTimeInMilliseconds(void) const
  {
    return (int) (GetTime() * 1000.0);
  }


  //-------------------------------------------------------
  /// Sets current timer value [ms].
  /// Timer-value = time which has passed since timer was started.
  //-------------------------------------------------------
  void Timer::SetTimeInMilliseconds(int t)
  {
    SetTime((float) (t/1000.0));
  }


  //-------------------------------------------------------
  /// Returns speed of timer.
  /// Examples: 0.5 ... timer runs half as fast as system-clock
  ///           1.0 ... timer runs synchroneous to system-clock
  ///           2.0 ... timer runs twice as fast as system-clock
  //-------------------------------------------------------
  float Timer::GetSpeed() const
  {
    return speedFactor;
  }


  //-------------------------------------------------------
  /// Sets speed of timer.
  /// Examples: 0.5 ... timer runs half as fast as system-clock
  ///           1.0 ... timer runs synchroneous to system-clock
  ///           2.0 ... timer runs twice as fast as system-clock
  //-------------------------------------------------------
  void Timer::SetSpeed(float s)
  {
    // A factor around 0 is currently not supported
    ASSERT(std::fabs(s) > 0.001);
    // Adjust time
    time *= speedFactor;  // remove effect of old speedFactor
    time /= s;            // apply new factor
    speedFactor = s;
  }
}
