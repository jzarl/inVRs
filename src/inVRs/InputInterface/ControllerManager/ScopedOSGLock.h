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

#ifndef _SCOPEDOSGLOCK_H_
#define _SCOPEDOSGLOCK_H_

#include "../../SystemCore/Platform.h"
#include <OpenSG/OSGLock.h>

/**
 * Helper class for OSG-locks which automatically releases the lock when the
 * object is deleted
 */
class ScopedOSGLock {
public:
	ScopedOSGLock(OSG::Lock* lock) :
		_lock(lock) {
		if (_lock) {
#if OSG_MAJOR_VERSION >= 2
			_lock->acquire();
#else //OpenSG1:
			_lock->aquire();
#endif
		}
	}

	~ScopedOSGLock() {
		if (_lock) {
			_lock->release();
		}
	}

private:
	ScopedOSGLock(const ScopedOSGLock& src);
	ScopedOSGLock& operator=(const ScopedOSGLock& rhs);

	OSG::Lock* _lock;
};

#endif /* SCOPEDOSGLOCK_H_ */
