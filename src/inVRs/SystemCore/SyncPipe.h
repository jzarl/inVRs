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

#ifndef SYNCPIPE_H
#define SYNCPIPE_H

#include <deque>
#include <string.h>
#include <vector>

#include <OpenSG/OSGLock.h>
#include <OpenSG/OSGThreadManager.h>
#include <OpenSG/OSGThread.h>


template<class T>
class SyncPipe {
public:
	SyncPipe();
	virtual ~SyncPipe();

	void push_back(T obj);
	T at(int idx);
	T pop_front();
	T erase(int idx);
	int size();
	void clear();
	std::deque<T>* makeCopyAndClear();
	std::vector<T>* makeCopyAsVectorAndClear();

protected:
	std::deque<T> objs;
#if OSG_MAJOR_VERSION >= 2
	OSG::LockRefPtr lock;	
#else //OpenSG1:
	OSG::Lock* lock;
#endif
	void initLock();
};

template<class T>
SyncPipe<T>::SyncPipe() {
	lock = NULL;
}

template<class T>
SyncPipe<T>::~SyncPipe() {

}

template<class T>
void SyncPipe<T>::push_back(T obj) {
	if (obj == NULL)
		return;
	if (lock == NULL)
		initLock();
#if OSG_MAJOR_VERSION >= 2
	lock->acquire();
#else //OpenSG1:
	lock->aquire();
#endif
	objs.push_back(obj);
	lock->release();
}

template<class T>
T SyncPipe<T>::erase(int idx) {
	T ret;
	if (lock == NULL)
		initLock();
#if OSG_MAJOR_VERSION >= 2
	lock->acquire();
#else //OpenSG1:
	lock->aquire();
#endif
	if (idx >= (int)objs.size()) {
		ret = NULL;
	} else {
		ret = objs[idx];
		objs.erase(objs.begin() + idx);
	}
	lock->release();
	return ret;
}

template<class T>
T SyncPipe<T>::at(int idx) {
	T ret;
	if (lock == NULL)
		initLock();
#if OSG_MAJOR_VERSION >= 2
	lock->acquire();
#else //OpenSG1:
	lock->aquire();
#endif
	if (idx >= (int)objs.size())
		ret = NULL;
	else
		ret = objs[idx];
	lock->release();
	return ret;
}

template<class T>
T SyncPipe<T>::pop_front() {
	T ret;
	if (lock == NULL)
		initLock();
#if OSG_MAJOR_VERSION >= 2
	lock->acquire();
#else //OpenSG1:
	lock->aquire();
#endif
	if (objs.empty())
		ret = NULL;
	else {
		ret = objs[0];
		objs.pop_front();
	}
	lock->release();
	return ret;
}

template<class T>
int SyncPipe<T>::size() {
	int ret;
	if (lock == NULL)
		initLock();
#if OSG_MAJOR_VERSION >= 2
	lock->acquire();
#else //OpenSG1:
	lock->aquire();
#endif
	ret = objs.size();
	lock->release();
	return ret;
}

template<class T>
std::deque<T>* SyncPipe<T>::makeCopyAndClear() {
	int i;
	std::deque<T>* ret = new std::deque<T>;
	if (lock == NULL)
		initLock();
#if OSG_MAJOR_VERSION >= 2
	lock->acquire();
#else //OpenSG1:
	lock->aquire();
#endif
	for (i = 0; i < (int)objs.size(); i++)
		ret->push_back(objs[i]);
	objs.clear();
	lock->release();
	return ret;
}

template<class T>
std::vector<T>* SyncPipe<T>::makeCopyAsVectorAndClear() {
	int i;
	std::vector<T>* ret = new std::vector<T>;
	if (lock == NULL)
		initLock();
#if OSG_MAJOR_VERSION >= 2
	lock->acquire();
#else //OpenSG1:
	lock->aquire();
#endif
	for (i = 0; i < (int)objs.size(); i++)
		ret->push_back(objs[i]);
	objs.clear();
	lock->release();
	return ret;
}

template<class T>
void SyncPipe<T>::initLock() {
	char buffer[200];
	sprintf(buffer, "SyncPipe:%p", this);
	// 	printf("-------- BUFFER = %s\n", buffer);
#if OSG_MAJOR_VERSION >= 2
	lock = OSG::dynamic_pointer_cast<OSG::Lock> (OSG::ThreadManager::the()->getLock(buffer,false));	
#else //OpenSG1:
	lock = dynamic_cast<OSG::Lock*> (OSG::ThreadManager::the()->getLock(buffer));
#endif
}

template<class T>
void SyncPipe<T>::clear() {
	if (lock == NULL)
		initLock();
#if OSG_MAJOR_VERSION >= 2
	lock->acquire();
#else //OpenSG1:
	lock->aquire();
#endif
	objs.clear();
	lock->release();
}

#endif
