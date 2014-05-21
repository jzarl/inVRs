#ifndef _THREADQUEUE_H_
#define _THREADQUEUE_H_

/***************************************************************************
 *   Copyright (C) 2007 by Thomas Webernorfer                              *
 *   thomas_weberndorfer@hotmail.com                                       *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <vector>

/**
 * \brief This is the abstract base class for asynchronous tasks.
 *
 * The aim of this class is providing CPU-time periodically without
 * interfering the other parts of the program. This is usefull for
 * animating widgets.
 *
 * The animation has to use this class as superclass, which allows 
 * registering such object at the ThreadQueue -class. The 
 * ThreadQueue -class will call the run( int time )-method
 * periodically.
 *
 * @date 9th of May 2007
 * @author Thomas Weberndorfer
 *
 **/
class QueuedThread
{

    public:

        /**
         * This method will be called by the ThreadQueue -class, if this object
         * has been registered using ThreadQueue::addToThreadQueue( QueuedThread *t ).
         *
         * This method is abstract. Overwrite it and perform tasks periodically.
         *
         * @param time This is a timestamp measured in milliseconds.
         * @return Return true, if this method should be called again by the ThreadQueue -class.
         *         If you return false, this object will be deleted.
         **/
        virtual bool run ( int time ) = 0;

        /**
         * There is nothing to clean up in an abstract class ...
         **/
        virtual ~QueuedThread() {}

};

/**
 * \brief This class manages a thread queue, which performs tasks periodically.
 *
 * The ThreadQueue is used to perform asynchronous tasks by on own thread.
 * These tasks should do their job in small units, which are executed each time,
 * when the ThreadQueue calls the run-method of the QueuedThread -object.
 *
 * When the run-method returns false, the QueuedThread -object will be deleted.
 *
 * The thread used for the run-method-calls is synchronized to OpenSG and
 * therefore changes to the SceneGraph -objects are no problem.
 *
 * @date 9th of May 2007
 * @author Thomas Weberndorfer
 *
 **/
class ThreadQueue
{

    private:

        // This list hold the QueuedThread-objects
        static std::vector<QueuedThread*> store;
        // The running-flag indicates, that the ThreadQueue works
        static bool running;
        // This method calls the run-methods
        static void work ( int time );

    public:

       /**
        * This method adds an object of type QueuedThread to the ThreadQueue. The
        * QueuedThread::run -method of this object will be called every 50 milliseconds until
        * false is returned.
        * @param thread Pointer to an object, which wants to enter the threadqueue.
        **/
       static void addToThreadQueue ( QueuedThread *thread );

};

#endif
