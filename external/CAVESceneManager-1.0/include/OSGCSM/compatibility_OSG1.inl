#include <OpenSG/OSGThread.h>
#include <OpenSG/OSGBaseFunctions.h>

namespace OSGCompat
{

inline void clearChangelist()
{
	OSG::Thread::getCurrentChangeList()->clearAll();
}

inline void setGlutId(pointer_traits<OSG::GLUTWindow>::RecPtr window, int id)
{
	window->setId(id);
}

inline void addServer(pointer_traits<OSG::MultiDisplayWindow>::RecPtr mwin, const std::string& server)
{
	mwin->getServers().push_back(server);
}

inline void clearForegrounds(pointer_traits<OSG::Viewport>::RecPtr vp)
{
	vp->getForegrounds().clear();
}

inline void addForeground(pointer_traits<OSG::Viewport>::RecPtr vp, pointer_traits<OSG::Foreground>::RecPtr fg)
{
	vp->getForegrounds().push_back(fg);
}

inline int getViewportCount(pointer_traits<OSG::Window>::RecPtr win)
{
	return win->getPort().size();
}

inline void removeForeground(pointer_traits<OSG::Viewport>::RecPtr vp, pointer_traits<OSG::Foreground>::RecPtr fg)
{
	osg::MFForegroundPtr::iterator it = vp->getForegrounds().find(fg);
	vp->getForegrounds().erase(it);
}

inline OSG::Real32 degreeToRad(OSG::Real32 deg)
{
	return osg::deg2rad(90.f);
}

template <typename ToType, typename FromType>
inline ToType osg_cast(FromType src)
{
	return ToType::dcast(src);
}

inline void addSurfacePoint(pointer_traits<OSG::ProjectionCameraDecorator>::RecPtr camDec, const OSG::Pnt3f& pnt)
{
	camDec->getSurface().push_back(pnt);
}

inline OSG::Pnt3f& editSurfacePoint(pointer_traits<OSG::ProjectionCameraDecorator>::RecPtr camDec, int idx)
{
    return camDec->getSurface(idx);
}

template <typename T>
inline void beginEditCP(const T& x)
{
    OSG::beginEditCP(x);
}

template <typename T>
inline void endEditCP(const T&x)
{
    OSG::endEditCP(x);
}

template <typename T>
inline void addRefCP(const T& x)
{
    OSG::addRefCP(x);
}

template <typename T>
inline void subRefCP(const T& x)
{
    OSG::subRefCP(x);
}

template <typename T, typename R>
inline void beginEditCP(const T&x, const R&y )
{
    OSG::beginEditCP(x,y);
}

template <typename T, typename R>
inline void endEditCP(const T&x, const R& y)
{
    OSG::endEditCP(x,y);
}

}
