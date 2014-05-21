#ifndef SHADOWVIEWPORTMGR_H_
#define SHADOWVIEWPORTMGR_H_

#include <OpenSG/OSGConfig.h>

#if OSG_MAJOR_VERSION < 2

#include <OSGCSM/viewportmgr.h>

#include <OpenSG/OSGShadowViewport.h>

#include <list>

//set defaults
	// shadow
	//beginEditCP(svp);
	//svp->setOffFactor(4.0);
	//svp->setOffBias(8.0);
	//svp->setGlobalShadowIntensity(0.9);
	//svp->setMapSize(1024);
	//svp->setShadowMode(ShadowViewport::STD_SHADOW_MAP);
	////svp->setShadowColor(Color4f(0.1,0.1,0.1,1.0));
	//svp->setShadowSmoothness(0.0);
	//OSGCompat::endEditCP  (svp);

class ShadowViewportMgr : public OSGCSM::IViewportMgr
{
public:
	ShadowViewportMgr();
	~ShadowViewportMgr();

	virtual OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr createMonoViewport();
	virtual OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr createStereoViewport(bool isLeftEye);
	bool destroyViewport(OSGCompat::pointer_traits<OSG::Viewport>::RecPtr vp);

	virtual void enableShadows();

	virtual void addShadowLightSource(OSGCompat::pointer_traits<OSG::Node>::RecPtr light);
	virtual void removeShadowLightSource(OSGCompat::pointer_traits<OSG::Node>::RecPtr light);
	virtual void addExclusionFromShadow(OSGCompat::pointer_traits<OSG::Node>::RecPtr exclude);
	virtual void removeExclusionFromShadow(OSGCompat::pointer_traits<OSG::Node>::RecPtr include);
	virtual void setShadowOffBias(const OSG::Real32 &value);
	virtual void setShadowOffFactor(const OSG::Real32 &value);
	virtual void setShadowMapSize(const OSG::UInt32 &value);
	virtual void setShadowMode(const OSG::UInt32 &value);
	virtual void setShadowSmoothness(const OSG::Real32 &value);
	virtual void setShadowOn(const bool &value);
	virtual void setShadowAutoSearchForLights(const bool &value);
	virtual void setShadowGlobalShadowIntensity(const OSG::Real32 &value);
	virtual void setShadowAutoExcludeTransparentNodes(const bool &value);

private:
	std::list<OSG::ShadowViewportPtr> m_shadowViewports;
	typedef std::list<OSG::ShadowViewportPtr>::iterator SVPIterator;
};

#endif
#endif
