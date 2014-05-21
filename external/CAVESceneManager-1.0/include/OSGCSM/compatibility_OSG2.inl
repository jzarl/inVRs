#include <OpenSG/OSGThread.h>
#include <OpenSG/OSGBaseFunctions.h>

namespace OSGCompat
{

inline void clearChangelist()
{
	OSG::Thread::getCurrentChangeList()->clear();
}

inline void setGlutId(pointer_traits<OSG::GLUTWindow>::RecPtr window, int id)
{
	window->setGlutId(id);
}

inline void addServer(pointer_traits<OSG::MultiDisplayWindow>::RecPtr mwin, const std::string& server)
{
	mwin->editMFServers()->push_back(server);
}

inline void clearForegrounds(pointer_traits<OSG::Viewport>::RecPtr vp)
{
	vp->clearForegrounds();
}

inline void addForeground(pointer_traits<OSG::Viewport>::RecPtr vp, pointer_traits<OSG::Foreground>::RecPtr fg)
{
	vp->addForeground(fg);
}

inline int getViewportCount(pointer_traits<OSG::Window>::RecPtr win)
{
	return win->getMFPort()->size();
}

inline void removeForeground(pointer_traits<OSG::Viewport>::RecPtr vp, pointer_traits<OSG::Foreground>::RecPtr fg)
{
	vp->removeObjFromForegrounds(fg);
}

inline OSG::Real32 degreeToRad(OSG::Real32 deg)
{
	return OSG::osgDegree2Rad(deg);
}

template <typename ToType, typename FromType>
inline ToType osg_cast(FromType src)
{
	return dynamic_cast<ToType>(src);
}

inline void addSurfacePoint(pointer_traits<OSG::ProjectionCameraDecorator>::RecPtr camDec, const OSG::Pnt3f& pnt)
{
	camDec->editMFSurface()->push_back(pnt);
}

inline OSG::Pnt3f& editSurfacePoint(pointer_traits<OSG::ProjectionCameraDecorator>::RecPtr camDec, int idx)
{
	return camDec->editSurface(idx);
}

template <typename T>
inline void beginEditCP(const T& x)
{
}

template <typename T>
inline void endEditCP(const T&x)
{
	//OSG::commitChanges(); //?
}

template <typename T>
inline void addRefCP(const T& x)
{
}

template <typename T>
inline void subRefCP(const T& x)
{
}

template <typename T, typename R>
inline void beginEditCP(const T&x, const R&y )
{
}

template <typename T, typename R>
inline void endEditCP(const T&x, const R& y)
{
}

}
