#ifndef _SCENEGRAPH_H_
#define _SCENEGRAPH_H_

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

#include <OpenSG/OSGTransform.h>

/**
 * Hide the OSG-type Line.
 **/
typedef OSG::Line Ray;

/**
 * Hide the OSG-type NodePtr.
 **/
typedef OSG::NodePtr SGPtr;

/**
 * \brief This class encapsulates all the OpenSG-stuff.
 *
 * This class handels all the scenegraph-depending stuff. Therefore no other class,
 * used for the 3dMenu-core-package, needs to include any scenegraph-headers.
 *
 * This class creates an OpenSG transform-node and attaches an OpenSG group-node.
 * All nodes, which will be added with the methods mergeScene(SceneGraph *scene),
 * loadModel(const char* filename) and addNodePtr(SGPtr child), will be
 * added to this group node. The other methods manipulate the transform-node.
 *
 * If an object of this class has been deleted, all OpenSG-nodes, which have been
 * created by this object, will be removed from the scene by the destructor
 * automatically.
 *
 * This class provides also picking-functionality, which consists of two steps:
 *
 * - At first a SceneGraph -object hast to become "pickable". Therefore
 *   some visible nodes have to be added to the SceneGraph -object and a pointer
 *   has to be registered using the method setPickingPointer( void *p ).
 *
 * - If someone wants to pick into the scene, the method
 *   getPickingPointer( Ray &ray ) must be called. This method tries to
 *   find a visible object und performs a lockup for finding the
 *   SceneGraph -object to which the visible object belongs. If a SceneGraph -object
 *   has been found, the method returns the registered pointer. If no visible
 *   object had been hit by the ray, NULL will be returned. In case
 *   that a SceneGraph -object has no pointer registered, the lockup-mechanism
 *   tries to find a parent- SceneGraph -object, which has a pointer registered.
 *
 * @date 23rd of March 2007
 * @author Thomas Weberndorfer
 *
 **/
class SceneGraph
{

    private:

        // This is the root-node of the intern OpenSG-scene.
        // It has a transform-core embedded.
        OSG::NodePtr root;
        // This node is attached to the root-node. It has a group-core embedded.
        OSG::NodePtr transformednode;
        // This is the transform-object used in the transform-core of the root-node.
        OSG::TransformPtr transform;
        // Just a simple helper function, for modifying the
        // matrix of the transform-object above.
        void multMatrix ( OSG::Matrix &matrix );
        // Store the center of the SceneGraph. The center is needed for method setRotate().
        OSG::Pnt3f center;

    public:

        /**
         * The constructor creates a root-node containing a transform-core and
         * attaches a transformed-node containing a group-core.
         **/
        SceneGraph();

        /**
         * The destructor removes all OpenSG-nodes
         * from the scenegraph to which the nodes have been added.
         **/
        virtual ~SceneGraph();

        /**
         * This method adds a OpenSG-node to the internal group-node.
         * @param child This node will be added to the intern group-node.
         **/
        void addNodePtr ( SGPtr child );

        /**
         * This method returns the internal root-node.
         * @return a reference to the internal root-node
         **/
        SGPtr getNodePtr();

        /**
         * This method is used for picking. It assignes a pointer to
         * the SceneGraph -object. 
         * @param p The pointer, which should be assigned.
         **/
        void setPickingPointer ( void *p );

        /**
         * This method is used for picking. If the ray hits a
         * SceneGraph -object, it returns the previously assigned pointer.
         * @param ray This ray will be traced to find any hit
         *             SceneGraph -objects.
         * @return It returns the assigned pointer of the first
         *         SceneGraph -object, which have been hit by the ray or
         *         NULL if nothing has been hit.
         **/
        void* getPickingPointer ( Ray &ray );

        /**
         * This method scales all objects, which have been added to
         * the SceneGraph -object.
         * @param x     scale in x-direction by factor x
         * @param y     scale in y-direction by factor y
         * @param z     scale in z-direction by factor z
         **/
        void setScale ( float x, float y, float z );

        /**
         * This method translates all objects, which have been added to
         * the SceneGraph -object.
         * @param x     translate in x-direction by the value of x
         * @param y     translate in y-direction by the value of y
         * @param z     translate in z-direction by the value of z
         **/
        void setTranslate ( float x, float y, float z );

        /**
         * This method rotates all objects, which have been added to
         * the SceneGraph -object. The parameters x, y and z are building 
         * a vector, which is used as rotation-axis.
         * @param x This is the x value of the rotation-axis.
         * @param y This is the y value of the rotation-axis.
         * @param z This is the z value of the rotation-axis.
         * @param angle The added objects will be rotated by the value of angle.
         *              This value is given in degrees.
         **/
        void setRotate ( float x, float y, float z, float angle );

        /**
         * This method loads a scene from a file and adds it to the
         * internal group-node. All formats, which are accepted by
         * OpenSG, can be used to store and load the scene.
         * @param filename This is the filename of the scene,
         *        which should be loaded.
         **/
        void loadModel ( const char* filename );

        /**
         * This method adds a scene of another SceneGraph -object
         * to this SceneGraph -object.
         * @param scene This has to be a reference to another SceneGraph -object.
         **/
        void mergeScene ( SceneGraph *scene );

        /**
         * This method sets the visibility of the SceneGraph -object.
         * @param vis Pass true to make the SceneGraph -object visible.
         *            Pass false to make the SceneGraph -object invisible.
         **/
        void setVisibility ( bool vis );

        /**
         * This method sets the transparency of the SceneGraph -object.
         * @param alpha This is the alpha value of the transparency.
         *              Valid values are [0 .. 1].
         **/
        void setTransparency ( float alpha );

};

#endif
