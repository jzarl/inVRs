
#include "shadowviewportmgr.h"

OSG_USING_NAMESPACE

#if OSG_MAJOR_VERSION < 2

ShadowViewportMgr::ShadowViewportMgr()
{
}

ShadowViewportMgr::~ShadowViewportMgr()
{
}

OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr 
ShadowViewportMgr::createMonoViewport()
{
	OSG::ShadowViewportPtr svp = OSG::ShadowViewport::create();
	m_shadowViewports.push_back(svp);

	return OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr(svp);
}

OSGCompat::pointer_traits<OSG::Viewport>::TransitPtr 
ShadowViewportMgr::createStereoViewport(bool isLeftEye)
{
	return createMonoViewport();
}

bool ShadowViewportMgr::destroyViewport(OSGCompat::pointer_traits<OSG::Viewport>::RecPtr vp)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		if ( (*it) == vp )
		{
			m_shadowViewports.erase(it);
			return true;
		}
	}
	return false;
}

void ShadowViewportMgr::enableShadows()
{
	setShadowOn( true );
}

void ShadowViewportMgr::addShadowLightSource(OSGCompat::pointer_traits<OSG::Node>::RecPtr light)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->getLightNodes().push_back(light);
		OSGCompat::endEditCP(*it);
	}
}

void ShadowViewportMgr::removeShadowLightSource(OSGCompat::pointer_traits<OSG::Node>::RecPtr light)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->getLightNodes().erase((*it)->getLightNodes().find(light));
		OSGCompat::endEditCP(*it);
	}
}

void ShadowViewportMgr::addExclusionFromShadow(OSGCompat::pointer_traits<OSG::Node>::RecPtr exclude)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->getExcludeNodes().push_back(exclude);
		OSGCompat::endEditCP(*it);
	}
}

void ShadowViewportMgr::removeExclusionFromShadow(OSGCompat::pointer_traits<OSG::Node>::RecPtr include)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->getExcludeNodes().erase((*it)->getExcludeNodes().find(include));
		OSGCompat::endEditCP(*it);
	}
}

void ShadowViewportMgr::setShadowOffBias(const Real32 &value)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->setOffBias(value);
		OSGCompat::endEditCP(*it);
	}
}

void ShadowViewportMgr::setShadowOffFactor(const Real32 &value)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->setOffFactor(value);
		OSGCompat::endEditCP(*it);
	}
}

void ShadowViewportMgr::setShadowMapSize(const UInt32 &value)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->setMapSize(value);
		OSGCompat::endEditCP(*it);
	}
}

void ShadowViewportMgr::setShadowMode(const UInt32 &value)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->setShadowMode(value);
		OSGCompat::endEditCP(*it);
	}
}

void ShadowViewportMgr::setShadowSmoothness(const Real32 &value)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->setShadowSmoothness(value);
		OSGCompat::endEditCP(*it);
	}
}

void ShadowViewportMgr::setShadowOn(const bool &value)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->setShadowOn(value);
		OSGCompat::endEditCP(*it);
	}
}

void ShadowViewportMgr::setShadowAutoSearchForLights(const bool &value)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->setAutoSearchForLights(value);
		OSGCompat::endEditCP(*it);
	}
}

void ShadowViewportMgr::setShadowGlobalShadowIntensity(const Real32 &value)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->setGlobalShadowIntensity(value);
		OSGCompat::endEditCP(*it);
	}
}

void ShadowViewportMgr::setShadowAutoExcludeTransparentNodes(const bool &value)
{
	for (SVPIterator it(m_shadowViewports.begin()); it != m_shadowViewports.end(); ++it)
	{
		OSGCompat::beginEditCP(*it);
		(*it)->setAutoExcludeTransparentNodes(value);
		OSGCompat::endEditCP(*it);
	}
}

#endif
