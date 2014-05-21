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

#ifndef _PHYSICSOBJECTID_H
#define _PHYSICSOBJECTID_H

/** Identifier class used to identify PhysicsObjects in the Physics module.
 * The PhysicsObjectID is an Identifier composed of an id for the type of
 * the object and the instance of the object. The type will be used as an
 * identifier for the class of which the object is an instance of. The
 * instance part will be a unique class instance number which has to be managed
 * by the user so that this id is really unique inside the class.
 */
class PhysicsObjectID
{
public:

//******************//
// PUBLIC METHODS: *//
//******************//

	/** Empty Constructor.
	 * Is needed to allow uninitialized variables.
	 */
	PhysicsObjectID();

	/** Constructor sets Identifier.
	 * The constructor has to be called with an identifier for the type of the
	 * PhysicsObject and the instance of the Object.
	 * @param typeID identifier for the class type
	 * @param instanceID unique class instance number
	 */
	PhysicsObjectID(unsigned int classTypeID, unsigned int classInstanceID);

	/** Getter for the class type identifier.
	 * @return class type of the PhysicsObject
	 */
	unsigned int getTypeID();

	/** Getter for the instance ID.
	 * @return instance of the PhysicsObject inside the class
	 */
	unsigned int getInstanceID();

	/** Assignment operator.
	 * Assigns the current PhysicsObjectID the values of the passed
	 * PhysicsObjectID.
	 * @param id PhysicsObjectID which should be used as template
	 * @return reference to the current PhysicsObjectID
	 */
	PhysicsObjectID& operator=(PhysicsObjectID id);

	/** Equal to operator.
	 * Returns if the current PhysicsObjectID is equal to the passed
	 * PhysicsObjectID.
	 * @param id PhysicsObjectID to compare with
	 * @return true if the two PhysicsObjectIDs are the same, false otherwise
	 */
	bool operator==(PhysicsObjectID& id);

	/** Less than operator.
	 * Returns if the current PhysicsObjectID is less than the passed
	 * PhysicsObjectID.
	 * @param id PhysicsObjectID to compare with
	 * @return true if the current PhysicsObjectID is lower, false otherwise
	 */
	bool operator<(PhysicsObjectID& id);

	/** Greater than operator.
	 * Returns if the curren PhysicsObjectID is greater than the passed
	 * PhysicsObjectID.
	 * @param id PhysicsObjectID to compare with
	 * @return true if the current PhysicsObjectID is higher, false otherwise
	 */
	bool operator>(PhysicsObjectID& id);

protected:

//*********************//
// PROTECTED MEMBERS: *//
//*********************//
	
	/// identifier for the class type
	unsigned int typeID;
	/// class instance number of the object
	unsigned int instanceID;

}; // PhysicsObjectID

#endif // _PHYSICSOBJECTID_H
