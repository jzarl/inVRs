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

#include <OpenSG/OSGConfig.h>
#if (OSG_MAJOR_VERSION < 2)
# include <OpenSG/OSGSimpleAttachments.h>
#else
# include "OSGNameAttachment.h"
# include "OSGVoidPAttachment.h"
#endif
#include <OpenSG/OSGSceneFileHandler.h>
#include <OpenSG/OSGIntersectAction.h>
#include <OpenSG/OSGMakeTransparentGraphOp.h>
#include <OpenSG/OSGMergeGraphOp.h>
#include <OpenSG/OSGStripeGraphOp.h>
#include <OpenSG/OSGMaterialMergeGraphOp.h>
#include <OpenSG/OSGGraphOpSeq.h>
#include <OpenSG/OSGGLUT.h>
#include <sstream>

#include <inVRs/SystemCore/Platform.h>
#include "SceneGraph.h"
#include "Config.h"

OSG_USING_NAMESPACE;

SceneGraph::SceneGraph()
{
    // create the root-node containing a transform-core
    // and a transformed-node containing a group-core
    transform = Transform::create();
    root = Node::create();
    transformednode = Node::create();
    beginEditCP ( transformednode, Node::CoreFieldMask );
    transformednode->setCore ( Group::create() );
    endEditCP ( transformednode, Node::CoreFieldMask );
    beginEditCP ( root, Node::CoreFieldMask | Node::ChildrenFieldMask );
    root->setCore ( transform );
    root->addChild ( transformednode );
    endEditCP ( root, Node::CoreFieldMask | Node::ChildrenFieldMask );
}

SceneGraph::~SceneGraph()
{
    // remove the root-node from its parent
    NodePtr parent = root->getParent();
    if ( parent != NullFC )
    {
        // Parent found
        beginEditCP ( parent, Node::ChildrenFieldMask );
        parent->subChild ( root );
        endEditCP ( parent, Node::ChildrenFieldMask );
    }
}

void SceneGraph::loadModel ( const char* filename )
{
    if ( filename != NULL ) {
#if OSG_MAJOR_VERSION >= 2
        GraphOpSeq *graphop = GraphOpSeq::create().get();
        graphop->addGraphOp(MergeGraphOp::create().get());
        graphop->addGraphOp(StripeGraphOp::create().get());
        graphop->addGraphOp(MaterialMergeGraphOp::create().get());
        addNodePtr ( SceneFileHandler::the()->read(filename, graphop) );
#else //OpenSG1:
        GraphOpSeq *graphop = new GraphOpSeq;
        graphop->addGraphOp(new MergeGraphOp);
        graphop->addGraphOp(new StripeGraphOp);
        graphop->addGraphOp(new MaterialMergeGraphOp);
        addNodePtr ( SceneFileHandler::the().read(filename, graphop) );
#endif
    }
}

void SceneGraph::addNodePtr ( OSG::NodePtr child )
{
    if ( child != NullFC )
    {
        beginEditCP ( transformednode, OSG::Node::ChildrenFieldMask );
        transformednode->addChild ( child );
        endEditCP ( transformednode, OSG::Node::ChildrenFieldMask );
#if OSG_MAJOR_VERSION >= 2
        BoxVolume &vol = transformednode->editVolume(true);
        vol.getCenter(center);
#else //OpenSG1:
        DynamicVolume &vol = transformednode->getVolume(true);
        vol.getCenter(center);
#endif
    }
}

NodePtr SceneGraph::getNodePtr()
{
    return root;
}

void SceneGraph::setPickingPointer ( void *p )
{
    if ( p != NULL )
    {
        VoidPAttachmentPtr pointer = VoidPAttachment::create();
#if OSG_MAJOR_VERSION >= 2
        beginEditCP ( pointer );
        pointer->editFieldPtr()->setValue ( p );
        endEditCP ( pointer );
#else //OpenSG1:
        beginEditCP ( pointer );
        pointer->getField().setValue ( p );
        endEditCP ( pointer );
#endif
        beginEditCP ( transformednode, Node::AttachmentsFieldMask );
        transformednode->addAttachment ( pointer );
        endEditCP ( transformednode, Node::AttachmentsFieldMask );
    }
}

void* SceneGraph::getPickingPointer ( OSG::Line &ray )
{
    // Create intersection action
#if OSG_MAJOR_VERSION >= 2
    IntersectActionRefPtr act = IntersectAction::create();
#else
    IntersectAction *act = IntersectAction::create();
#endif
    act->setLine ( ray );
    act->apply ( root );
    void *result = NULL;
    // Intersect
    if ( act->didHit() )
    {
        NodePtr hitnode = act->getHitObject();
        AttachmentPtr attachement;
        do
        {
            // Climb up in the OpenSG-tree until a noOSG_USING_NAMESPACEde with a fitting
            // attachement has been found.
            attachement = hitnode->findAttachment ( VoidPAttachment::getClassType() );
            hitnode = hitnode->getParent();
        }
        while ( attachement == NullFC );
        // Attachement with pointer found
        result = VoidPAttachmentPtr::dcast ( attachement )->getField().getValue();
    }
#if OSG_MAJOR_VERSION < 2
    delete act;
#endif
    return result;
}

void SceneGraph::multMatrix ( OSG::Matrix &matrix )
{
    beginEditCP ( transform, Transform::MatrixFieldMask );
#if OSG_MAJOR_VERSION >= 2
    transform->editMatrix().mult ( matrix );
#else
    transform->getMatrix().mult ( matrix );
#endif
    endEditCP ( transform, Transform::MatrixFieldMask );
}

void SceneGraph::setScale ( float x, float y, float z )
{
    Matrix matrix;
    matrix.setScale ( x, y, z );
    multMatrix ( matrix );
}

void SceneGraph::setTranslate ( float x, float y, float z )
{
    Matrix matrix;
    matrix.setTranslate ( x, y, z );
    multMatrix ( matrix );
}

void SceneGraph::setRotate ( float x, float y, float z, float angle )
{
    setTranslate( center[0] , center[1] , center[2] );
    Quaternion q;
    q.setValueAsAxisDeg ( x, y, z, angle );
    Matrix matrix;
    matrix.setRotate ( q );
    multMatrix ( matrix );
    setTranslate( -center[0] , -center[1] , -center[2] );
}

void SceneGraph::mergeScene ( SceneGraph *scene )
{
    if ( scene != NULL )
        addNodePtr ( scene->getNodePtr() );
}

void SceneGraph::setVisibility ( bool vis )
{
    beginEditCP ( root );
    root->setActive ( vis );
    endEditCP ( root );
}

void SceneGraph::setTransparency ( float alpha )
{
#if OSG_MAJOR_VERSION >= 2
    MakeTransparentGraphOpTransitPtr op = MakeTransparentGraphOp::create();
#else //OpenSG1:
    MakeTransparentGraphOp *op = new MakeTransparentGraphOp;
#endif
    std::ostringstream str;
    str << "transparency=" << (alpha*100);
    op->setParams ( str.str() );
    op->traverse ( root );
#if OSG_MAJOR_VERSION < 2
    delete op;
#endif
}
