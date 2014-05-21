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

#ifndef ATTACHMENTKEY_H_
#define ATTACHMENTKEY_H_

#include "../Platform.h"

class INVRS_SYSTEMCORE_API AttachmentKey {
public:
	/** 
	 * The Assignment operator assigns the values of the passed AttachmentKey to 
	 * the current AttachmentKey.
	 * @param key AttachmentKey which should be used as template
	 * @return reference to the current AttachmentKey
	 */
	AttachmentKey& operator=(AttachmentKey& key);

	/** 
	 * The Equal to operator returns if the current AttachmentKey is equal to 
	 * the passed AttachmentKey.
	 * @param key AttachmentKey to compare with
	 * @return true if the two AttachmentKeys are the same, false otherwise
	 */
	bool operator==(AttachmentKey& key);

	/** 
	 * The Less than operator returns if the current AttachmentKey is less than 
	 * the passed AttachmentKey.
	 * @param key AttachmentKey to compare with
	 * @return true if the current AttachmentKey is lower, false otherwise
	 */
	bool operator<(const AttachmentKey& key) const;

	/** 
	 * Greater than operator returns if the curren AttachmentKey is greater than 
	 * the passed AttachmentKey.
	 * @param key AttachmentKey to compare with
	 * @return true if the current AttachmentKey is higher, false otherwise
	 */
	bool operator>(const AttachmentKey& key) const;

public:
	/// ID of the Module which adds the Attachment
	unsigned moduleId;

	/// ID of the User for which the Attachment should be added
	unsigned userId;
}; // AttachmentKey

#endif /* ATTACHMENTKEY_H_ */
