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
 *                                Changes                                    *
 *                                                                           *
 *                                                                           *
 *                                                                           *
 *                                                                           *
\*---------------------------------------------------------------------------*/

#ifndef _SIMULATIONSTEPLISTENERINTERFACE_H
#define _SIMULATIONSTEPLISTENERINTERFACE_H

class Physics;

/** Interface for SimulationStepListener in Physics Module.
 * This interface can be used to implement a class which will always be notified
 * when the Physics Module initiates a simulation step. The <code>step</code>
 * callback method is always called from the Physics Thread in the step method
 * before the real simulation step occurs. This allows to update data which is
 * needed for the next simulation step. NOTE: The listener can be registered
 * out of the System Thread or out of the Physics Thread. An opportunity would
 * be to register the listener in the constructor of the object. When doing so
 * take care that the listener could be called before the object is added to
 * the simulation.
 */
class SimulationStepListenerInterface
{
public:

	/** Empty destructor.
	 */
	virtual ~SimulationStepListenerInterface(){};

protected:
	friend class Physics;

//*********************//
// PROTECTED METHODS: *//
//*********************//

	/** Callback function when Physics::step is called.
	 * This method is called from the Physics Thread when a simulation step
	 * will be initiated. The passed argument defines the timestep which will
	 * be simulated. The real simulation step takes place after this method
	 * call.
	 * @param dt timestep of the next simulation step
	 * @param simulationTime time of the simulation
	 */
	virtual void step(float dt, unsigned simulationTime) = 0;
	
}; // SimulationStepListenerInterface

#endif // _SIMULATIONSTEPLISTENERINTERFACE_H
