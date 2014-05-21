#ifndef VIEWPORTMGR_H_
#define VIEWPORTMGR_H_

#include "compatibility.h"

#include <OpenSG/OSGViewport.h>

namespace OSGCSM
{

class IViewportMgr
{
public:
	virtual ~IViewportMgr() {}
	virtual OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr createMonoViewport() = 0;
	virtual OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr createStereoViewport(bool isLeftEye) = 0;
	virtual bool destroyViewport(OSGCompat::pointer_traits<OSG::Viewport>::RecPtr vp) = 0;
};

}

#endif