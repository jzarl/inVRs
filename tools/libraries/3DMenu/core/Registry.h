#ifndef _REGISTRY_H_
#define _REGISTRY_H_

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

#include "ObjectStore.h"
#include "MenuObject.h"
#include "Animation.h"

/**
 * \brief This class provides a compatible callback-method for the Registry -class.
 *
 * Classes, which like to register themselves
 * at the Registry -class, have to use this class as superclass.
 *
 * This class is abstract, because it should be used like an interface.
 * You have to implement the method void invoke(const char *symbol).
 * Use the method
 * Registry::registerCallBack(CallBack *callback, const char* symbol)
 * for registering an object at the Registry -class.
 *
 * @date 21st of April 2007
 * @author Thomas Weberndorfer
 *
 **/
class CallBack
{

    public:

        /**
         * This method will be called by the Registry. Because an object can be
         * multiple registered using different symbols, the symbol which
         * triggered the invocation, is passed back to the registered object.
         * @param name The method invoke() has been triggered by an object 
         *             with this name.
         * @param value The value contains information about the reason why this
         *              the method invoke() has been called.
         **/
        virtual void invoke ( const char *name, const char *value ) = 0;

        /**
         * The destructor has nothing to do.
         **/
        virtual ~CallBack() {};

};

/**
 * This is the type-definition for static methods, which are used as
 * callback-methods with the registry.
 **/
typedef void ( *StaticCallBack ) ( const char *name, const char *value );

/**
 * \brief This class maps symbolic callback-names to "real" callbacks and manages animations.
 *
 * If an application needs a menu, it has to define an XML-file, explaining
 * which widgets are needed and where they should be placed. It is also
 * necessary to define which actions should be invoked by the widget.
 * Because the addresses of callback-methods are unknown at compile time, only
 * symbolic names can be used to describe what should happen, when someone
 * uses a widget. So somehow the symbolic names (short "symbols") must be
 * mapped on real callback-methods. And this is exactly the job of this class.
 *
 * This class maps callback-methods on symbolic names. These callback-methods
 * can be static methods or methods of objects:
 *
 * - For using a static method as callback-method, the signature of the method
 *   must have the shape of "void (*StaticCallBack)(const char *symbol)".
 *   Callback-methods of this kind can be registered by using the static method
 *   Registry::registerCallBack(StaticCallBack callback, const char* symbol).
 *
 * - Dynamic objects can be registered by this Registry -class, if they have used
 *   the class CallBack as super-class. These objects can be registered by the
 *   Registry -class using the static method
 *   Registry::registerCallBack(CallBack *callback, const char* symbol).
 *
 * These callbacks can be invoked by the static method
 * Registry::callCallBack(const char* symbol).
 * This method is used by the widgets. They parse the XML-file and store the
 * symbolic names for the callback. If someone uses these widgets, the callback
 * will be performed.
 *
 * For each callback-invocation also the symbol, which have been used for
 * Registry::callCallBack(const char* symbol) is passed to the called method.
 * This is usefull, if a callback-method is registered serveral times
 * with different symbols.
 *
 * This class is also used for storing references to Animation -objects. New animations
 * are added by the method Registry::registerAnimation( Animation *ani, const char* symbol).
 * These animations can be triggered using the methods
 * callAnimate( MenuObject *obj, const char* symbol ) and
 * callUnAnimate( MenuObject *obj, const char* symbol ).
 *
 * @date 21st of April 2007
 * @author Thomas Weberndorfer
 *
 **/
class Registry
{

    private:

        // These ObjectStores store the callback-references and animations according to the symbols.
        static ObjectStore<CallBack> callbackStore;
        static ObjectStore<Animation> animationStore;


        // This is a callback function, which deletes objects of type Container.
        static void callback ( CallBack *cb );

        /*
         * This is a private class, which stores a pointer to a static callback-method.
         * So objects of this class wrap static callback-methods, which allows administrating
         * all callback-methods in the same way.
         */
    class Container : public CallBack
        {

            public:
                // The constructor stores the reference.
                Container ( StaticCallBack callback ) : callback ( callback ) {};
                // This method invokes the reference.
                void invoke ( const char *name, const char *value ) { callback ( name, value ) ; }

            private:
                StaticCallBack callback;
        };

    public:

        /**
         * This method is used for registering a callback with its corresponding symbolic name.
         * @param callback  reference to the object with the callback-method
         * @param symbol    Pass the corresponding symbolic name.
         **/
        static void registerCallBack ( CallBack *callback, const char* symbol );

        /**
         * This method is used for registering a callback with its corresponding symbolic name.
         * @param callback  reference to a static callback-method
         * @param symbol    Pass the corresponding symbolic name.
         **/
        inline static void registerCallBack ( StaticCallBack callback, const char* symbol )
        {
            registerCallBack ( new Container ( callback ), symbol );
        }

        /**
         * This method invokes the callback-method, which has been bound to the symbolic name.
         * @param name    use the name of the calling MenuObject for this parameter
         * @param value   this indicates the reason for the call
         **/
        static void callCallBack ( const char* name, const char* value );

        /**
         * This method unregisters a registered callback-method. The typ of the
         * registered method (static or an object) is regarless.
         * @param symbol The registered method with this symbol will be unregistered.
         **/
        static void unregisterCallBack ( const char* symbol );

        /**
         * This method registers a new animation. Each Animation -object
         * has to be registered using an unique symbol.
         * @param ani referenz to the Animation -object
         * @param symbol the symbolic name of the Animation -object
         **/
        static void registerAnimation ( Animation *ani, const char* symbol );

        /**
         * This method performs an animation.
         * @param obj Perform the animation on this object.
         * @param symbol Use the animation, which has been registered with this symbol.
         **/
        static void callAnimate ( MenuObject *obj, const char* symbol );

        /**
         * This method cancels all effects of Registry::callAnimate(MenuObject *obj, const char* symbol),
         * which has to be called before.
         * @param obj Undo the animation for this object.
         * @param symbol The animation using this symbol will be undone.
         **/
        static void callUnAnimate ( MenuObject *obj, const char* symbol );

};

#endif
