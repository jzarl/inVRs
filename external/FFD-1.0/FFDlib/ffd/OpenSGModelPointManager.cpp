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
 *                           Project: FFD                                    *
 *                                                                           *
 * The FFD library was developed during a practical at the Johannes Kepler   *
 * University, Linz in 2009 by Marlene Hochrieser                            *
 *                                                                           *
\*---------------------------------------------------------------------------*/


#include "OpenSGModelPointManager.h"

OSG_USING_NAMESPACE

OpenSGModelPointManager::OpenSGModelPointManager(NodePtr node) :
    node(node),
    numOfGeometryNodes(0),
    model(0),
    isMasked(false)
{
    // TODO: get rid of tm as a member
    tm = Matrix::identity();
}

OpenSGModelPointManager::~OpenSGModelPointManager()
{
}

bool OpenSGModelPointManager::getMasked()
{
    return isMasked;
}

gmtl::AABoxf OpenSGModelPointManager::getAffectedVolume()
{
    return affectedVolume;
}

void OpenSGModelPointManager::collectModelPoints()
{
    numOfGeometryNodes = 0;

#if OSG_MAJOR_VERSION >= 2
	traverse(node,
        boost::bind(&OpenSGModelPointManager::getNumOfGeometryNodes, this,_1));
#else //OpenSG1:
	traverse(node,
        osgTypedMethodFunctor1ObjPtrCPtrRef
            <Action::ResultE, OpenSGModelPointManager, NodePtr>
             (this, &OpenSGModelPointManager::getNumOfGeometryNodes)
    );
#endif
    

    modelPoints.resize(numOfGeometryNodes);

    model = 0;
    tm = Matrix::identity();

#if OSG_MAJOR_VERSION >= 2
	traverse(node,
		boost::bind(&OpenSGModelPointManager::collectGeometryNodePoints, this, _1),
		boost::bind(&OpenSGModelPointManager::leave, this, _1, _2)
    );
#else //OpenSG1:
	 traverse(node,
        osgTypedMethodFunctor1ObjPtrCPtrRef
            <Action::ResultE, OpenSGModelPointManager, NodePtr>
             (this, &OpenSGModelPointManager::collectGeometryNodePoints),
        osgTypedMethodFunctor2ObjPtrCPtrRef
            <Action::ResultE, OpenSGModelPointManager, NodePtr, Action::ResultE>
             (this, &OpenSGModelPointManager::leave)
    );
#endif
   
}

void OpenSGModelPointManager::collectModelPointsMasked(const gmtl::AABoxf& aabb)
{
    // TODO: provide functionality to set the modelPointsAffected mask from
    // outside -> collect all points, give reference to the mask
    isMasked = true;
    affectedVolume = aabb;

    numOfGeometryNodes = 0;
#if OSG_MAJOR_VERSION >= 2
	traverse(node,
        boost::bind(&OpenSGModelPointManager::getNumOfGeometryNodes, this,_1));
#else //OpenSG1:
	traverse(node,
        osgTypedMethodFunctor1ObjPtrCPtrRef
            <Action::ResultE, OpenSGModelPointManager, NodePtr>
             (this, &OpenSGModelPointManager::getNumOfGeometryNodes)
    );
#endif
    

    modelPoints.resize(numOfGeometryNodes);
    modelPointsAffected.resize(numOfGeometryNodes);

    model = 0;
    tm = Matrix::identity();


#if OSG_MAJOR_VERSION >= 2
	traverse(node,
		boost::bind(&OpenSGModelPointManager::collectGeometryNodePoints, this, _1),
		boost::bind(&OpenSGModelPointManager::leave, this, _1, _2)
    );
#else //OpenSG1:
    traverse(node,
        osgTypedMethodFunctor1ObjPtrCPtrRef
            <Action::ResultE, OpenSGModelPointManager, NodePtr>
             (this, &OpenSGModelPointManager::collectGeometryNodePointsMasked),
        osgTypedMethodFunctor2ObjPtrCPtrRef
            <Action::ResultE, OpenSGModelPointManager, NodePtr, Action::ResultE>
             (this, &OpenSGModelPointManager::leave)
    );
#endif
}

void OpenSGModelPointManager::assignModelPoints()
{
    model = 0;
    tm = Matrix::identity();

#if OSG_MAJOR_VERSION >= 2
	traverse(node,
		boost::bind(&OpenSGModelPointManager::assignGeometryNodePoints, this, _1),
		boost::bind(&OpenSGModelPointManager::leave, this, _1, _2)
    );
#else //OpenSG1:
    traverse(node,
        osgTypedMethodFunctor1ObjPtrCPtrRef
            <Action::ResultE, OpenSGModelPointManager, NodePtr>
             (this, &OpenSGModelPointManager::assignGeometryNodePoints),
        osgTypedMethodFunctor2ObjPtrCPtrRef
            <Action::ResultE, OpenSGModelPointManager, NodePtr, Action::ResultE>
             (this, &OpenSGModelPointManager::leave)
    );
#endif
    
}

void OpenSGModelPointManager::assignModelPointsMasked()
{
    model = 0;
    tm = Matrix::identity();

#if OSG_MAJOR_VERSION >= 2
	traverse(node,
		boost::bind(&OpenSGModelPointManager::assignGeometryNodePointsMasked, this, _1),
		boost::bind(&OpenSGModelPointManager::leave, this, _1, _2)
    );
#else //OpenSG1:
    traverse(node,
        osgTypedMethodFunctor1ObjPtrCPtrRef
            <Action::ResultE, OpenSGModelPointManager, NodePtr>
             (this, &OpenSGModelPointManager::assignGeometryNodePointsMasked),
        osgTypedMethodFunctor2ObjPtrCPtrRef
            <Action::ResultE, OpenSGModelPointManager, NodePtr, Action::ResultE>
             (this, &OpenSGModelPointManager::leave)
    );
#endif
}

bool OpenSGModelPointManager::createSavepoint()
{
    modelPointsSavepoint = modelPoints;

    if (modelPointsSavepoint.size() == modelPoints.size())
        return true;

    return false;
}

bool OpenSGModelPointManager::restoreSavepoint()
{
    modelPoints = modelPointsSavepoint;

    if (modelPointsSavepoint.size() == modelPoints.size())
        return true;

    return false;
}

bool OpenSGModelPointManager::deleteSavepoint()
{
    modelPointsSavepoint.clear();
    return true;
}

#if OSG_MAJOR_VERSION >= 2
Action::ResultE OpenSGModelPointManager::getNumOfGeometryNodes(Node* const node)	
#else
Action::ResultE OpenSGModelPointManager::getNumOfGeometryNodes(NodePtr& node)
#endif
{
    GeometryPtr core = GeometryPtr::dcast(node->getCore());

    if (core != NullFC)
        ++numOfGeometryNodes;

    return Action::Continue;
}

// get modelpoints considering transformations
#if OSG_MAJOR_VERSION >= 2
Action::ResultE OpenSGModelPointManager::collectGeometryNodePoints(Node* const node)	
#else
Action::ResultE OpenSGModelPointManager::collectGeometryNodePoints(NodePtr& node)
#endif
{
    GeometryPtr core = GeometryPtr::dcast(node->getCore());

    // get the transformation of the transformnodes above
    TransformPtr tcore = TransformPtr::dcast(node->getCore());
    if (tcore != NullFC)
    {
		const Matrix curTMat = tcore->getMatrix();
        tm.mult(curTMat);
    }
    else if (core != NullFC)
    {
#if OSG_MAJOR_VERSION >= 2
		GeoPnt3fPropertyRecPtr pos = dynamic_cast<GeoPnt3fProperty *>(core->getPositions());
#else
        GeoPositions3fPtr pos = GeoPositions3fPtr::dcast(core->getPositions());
#endif
        modelPoints[model].resize(pos->size());

        OSG::Pnt3f currentPoint, transformedPoint;

        // transform each point by its transformationmatrix
        for(size_t point = 0; point < pos->size(); ++point)
        {
            currentPoint = pos->getValue(point);
            tm.multMatrixPnt(currentPoint, transformedPoint);
            modelPoints[model][point] = gmtl::Vec3f(transformedPoint[0],
                                                    transformedPoint[1],
                                                    transformedPoint[2]);
        }
        ++model;
    }

    return Action::Continue;
}

// get modelpoints considering transformations
#if OSG_MAJOR_VERSION >= 2
Action::ResultE OpenSGModelPointManager::collectGeometryNodePointsMasked(Node* const node)	
#else
Action::ResultE OpenSGModelPointManager::collectGeometryNodePointsMasked(NodePtr& node)
#endif
{
    GeometryPtr core = GeometryPtr::dcast(node->getCore());

    // get the transformation of the transformnodes above
    TransformPtr tcore = TransformPtr::dcast(node->getCore());
    if (tcore != NullFC)
    {
        const Matrix curTMat = tcore->getMatrix();
        tm.mult(curTMat);
    }
    else if (core != NullFC)
    {
#if OSG_MAJOR_VERSION >= 2
		GeoPnt3fPropertyRecPtr pos = dynamic_cast<GeoPnt3fProperty *>(core->getPositions());
#else
        GeoPositions3fPtr pos = GeoPositions3fPtr::dcast(core->getPositions());
#endif
        modelPoints[model].resize(pos->size());
        modelPointsAffected[model].resize(pos->size());

        OSG::Pnt3f currentPoint, transformedPoint;

        // transform each point in the given bounding box by its TM
        for(size_t point = 0; point < pos->size(); ++point)
        {
            currentPoint = pos->getValue(point);
            tm.multMatrixPnt(currentPoint, transformedPoint);

            if (gmtl::isInVolume(affectedVolume, gmtl::Point3f(
                                                        transformedPoint[0],
                                                        transformedPoint[1],
                                                        transformedPoint[2])))
            {
                modelPoints[model][point] = gmtl::Vec3f(transformedPoint[0],
                                                        transformedPoint[1],
                                                        transformedPoint[2]);
                modelPointsAffected[model][point] = true;
            }
            else
                modelPointsAffected[model][point] = false;
        }
        ++model;
    }

    return Action::Continue;
}

// set modelpoints considering transformations
#if OSG_MAJOR_VERSION >= 2
Action::ResultE OpenSGModelPointManager::assignGeometryNodePoints(Node* const node)	
#else
Action::ResultE OpenSGModelPointManager::assignGeometryNodePoints(NodePtr& node)
#endif
{
    GeometryPtr core = GeometryPtr::dcast(node->getCore());

    // apply the transformation of the transformnodes above
    TransformPtr tcore = TransformPtr::dcast(node->getCore());
    if (tcore != NullFC)
    {
		Matrix curTMatInv = tcore->getMatrix();
        tm.mult(curTMatInv);
    }
    else if (core != NullFC)
    {
        OSG::Pnt3f currentPoint, transformedPoint;
#if OSG_MAJOR_VERSION >= 2
		GeoPnt3fPropertyRecPtr pos = dynamic_cast<GeoPnt3fProperty *>(core->getPositions());
#else
        GeoPositions3fPtr pos = GeoPositions3fPtr::dcast(core->getPositions());
#endif

        // transform each point by its inverse transformationmatrix
        Matrix tminv = tm;
        tminv.inverse(tminv);
#if OSG_MAJOR_VERSION == 1
        beginEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);
#endif
            for(size_t point = 0; point < pos->size(); ++point)
            {
                currentPoint = OSG::Pnt3f(modelPoints[model][point][0],
                    modelPoints[model][point][1], modelPoints[model][point][2]);
                tminv.multMatrixPnt(currentPoint, transformedPoint);
                pos->setValue(transformedPoint, point);
            }
#if OSG_MAJOR_VERSION == 1
        endEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);
#endif

#if OSG_MAJOR_VERSION >= 2
		beginEditCP(core);
            core->setPositions(pos);
        endEditCP(core);
#else
		beginEditCP(core, Geometry::PositionsFieldMask);
            core->setPositions(pos);
        endEditCP(core, Geometry::PositionsFieldMask);
#endif 
        

        ++model;
    }

    return Action::Continue;
}

// TODO: get rid of geometry node counting (use push_back)
#if OSG_MAJOR_VERSION >= 2
Action::ResultE OpenSGModelPointManager::assignGeometryNodePointsMasked(Node* const node)	
#else
Action::ResultE OpenSGModelPointManager::assignGeometryNodePointsMasked(NodePtr& node)
#endif
{
    GeometryPtr core = GeometryPtr::dcast(node->getCore());

    // apply the transformation of the transformnodes above
    TransformPtr tcore = TransformPtr::dcast(node->getCore());
    if (tcore != NullFC)
    {
        Matrix curTMatInv = tcore->getMatrix();
        tm.mult(curTMatInv);
    }
    else if (core != NullFC)
    {
        OSG::Pnt3f currentPoint, transformedPoint;
#if OSG_MAJOR_VERSION >= 2
		GeoPnt3fPropertyRecPtr pos = dynamic_cast<GeoPnt3fProperty *>(core->getPositions());
#else
        GeoPositions3fPtr pos = GeoPositions3fPtr::dcast(core->getPositions());;
#endif

        // transform each point by its inverse transformationmatrix
        Matrix tminv = tm;
        tminv.inverse(tminv);
#if OSG_MAJOR_VERSION == 1
        beginEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);
#endif
            for(size_t point = 0; point < pos->size(); ++point)
            {
                if (modelPointsAffected[model][point])
                {
                    currentPoint = OSG::Pnt3f(modelPoints[model][point][0],
                                              modelPoints[model][point][1],
                                              modelPoints[model][point][2]);
                    tminv.multMatrixPnt(currentPoint, transformedPoint);
                    pos->setValue(transformedPoint, point);
                }
                else
                    currentPoint = pos->getValue(point);
            }
#if OSG_MAJOR_VERSION == 1
        endEditCP(pos, GeoPositions3f::GeoPropDataFieldMask);
#endif

#if OSG_MAJOR_VERSION >= 2
		beginEditCP(core);
            core->setPositions(pos);
        endEditCP(core);
#else
		beginEditCP(core, Geometry::PositionsFieldMask);
            core->setPositions(pos);
        endEditCP(core, Geometry::PositionsFieldMask);
#endif 
        

        ++model;
    }

    return Action::Continue;
}

#if OSG_MAJOR_VERSION >= 2
Action::ResultE OpenSGModelPointManager::recalculateNormals(Node* const node)	
#else
Action::ResultE OpenSGModelPointManager::recalculateNormals(NodePtr& node)
#endif
{
    GeometryPtr core = GeometryPtr::dcast(node->getCore());

    if (core != NullFC)
    {
#if OSG_MAJOR_VERSION >= 2
        beginEditCP(core);
            calcVertexNormals(core);
            calcFaceNormals(core);
        endEditCP(core);
#else
        beginEditCP(core, Geometry::NormalsFieldMask);
            calcVertexNormals(core);
            calcFaceNormals(core);
        endEditCP(core, Geometry::NormalsFieldMask);
#endif 
    }

    return Action::Continue;
}

#if OSG_MAJOR_VERSION >= 2
Action::ResultE OpenSGModelPointManager::leave(Node* const node, Action::ResultE result)
#else
Action::ResultE OpenSGModelPointManager::leave(NodePtr& node, Action::ResultE result)
#endif
{
    TransformPtr tcore = TransformPtr::dcast(node->getCore());
    if (tcore != NullFC)
    {
        Matrix curTMatInv = tcore->getMatrix();
        curTMatInv.inverse(curTMatInv);
        tm.mult(curTMatInv);
    }

    return result;
}
