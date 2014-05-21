#ifndef COMPATIBILITY_H_
#define COMPATIBILITY_H_

#include <string>

#include <OpenSG/OSGConfig.h>
#include <OpenSG/OSGBaseTypes.h>

#include <OpenSG/OSGGLUTWindow.h>
#include <OpenSG/OSGMultiDisplayWindow.h>
#include <OpenSG/OSGWindow.h>
#include <OpenSG/OSGViewport.h>
#include <OpenSG/OSGForeground.h>
#include <OpenSG/OSGProjectionCameraDecorator.h>
#include <OpenSG/OSGRenderAction.h>

#if OSG_MAJOR_VERSION < 2
#include <OpenSG/OSGRefPtr.h>
#endif

namespace OSGCompat
{
#if OSG_MAJOR_VERSION >= 2

	template<typename T>
	struct pointer_traits
	{
		typedef OSG::TransitPtr<T> TransitPtr;
        typedef OSG::RefCountPtr<T, OSG::RecordedRefCountPolicy> RecPtr;
		typedef T* OmnsPtr; //OMNS stands for "old: managed, new: simple", use where OpenSG 1.+ expects a managed/refcounted pointer and OpenSG 2.+ a simple C-pointer instead
		typedef OSG::RefCountPtr<T, OSG::MemObjRefCountPolicy> OsnmPtr; //OSNM stands for "old: simple, new: managed", reverse as above
	};

	// older versions of OpenSG used a special OSG::NullFC instead of NULL."
#ifdef HAS_CXX11_NULLPTR
	static const std::nullptr_t NullFC = nullptr;
#else
	static const int NullFC = 0;
#endif

#else

	template<typename T>
	struct pointer_traits
	{
        typedef typename T::Ptr Ptr;
        typedef Ptr RecPtr;
        typedef OSG::RefPtr<Ptr> RefPtr;
        typedef Ptr TransitPtr;
		typedef T* OsnmPtr;
        typedef Ptr OmnsPtr;
	};

    template<>
    struct pointer_traits<OSG::RenderAction>
    {
        typedef OSG::RenderAction* OsnmPtr;
    };

	static const osg::NullFieldContainerPtr& NullFC = osg::NullFC;
#endif
	void clearChangelist();
	void setGlutId(pointer_traits<OSG::GLUTWindow>::RecPtr, int id);
	void addServer(pointer_traits<OSG::MultiDisplayWindow>::RecPtr mwin, const std::string& server);
	void clearForegrounds(pointer_traits<OSG::Viewport>::RecPtr vp);
	void addForeground(pointer_traits<OSG::Viewport>::RecPtr vp, pointer_traits<OSG::Foreground>::RecPtr fg);
	int  getViewportCount(pointer_traits<OSG::Window>::RecPtr win);
	void removeForeground(pointer_traits<OSG::Viewport>::RecPtr vp, pointer_traits<OSG::Foreground>::RecPtr fg);
	OSG::Real32 degreeToRad(OSG::Real32 dgr);
	void addSurfacePoint(pointer_traits<OSG::ProjectionCameraDecorator>::RecPtr camDec, const OSG::Pnt3f& pnt);
	OSG::Pnt3f& editSurfacePoint(pointer_traits<OSG::ProjectionCameraDecorator>::RecPtr camDec, int idx);

    template <typename T>
    void beginEditCP(const T& x);

    template <typename T>
    void endEditCP(const T&x);

    template <typename T>
    void addRefCP(const T& x);

    template <typename T>
    void subRefCP(const T& x);

    template <typename T, typename R>
    void beginEditCP(const T&x, const R&y );

    template <typename T, typename R>
    void endEditCP(const T&x, const R& y);

	template <typename ToType, typename FromType>
	inline ToType osg_cast(FromType src);
};

#if OSG_MAJOR_VERSION >= 2
#include "compatibility_OSG2.inl"
#else
#include "compatibility_OSG1.inl"
#endif

#endif
