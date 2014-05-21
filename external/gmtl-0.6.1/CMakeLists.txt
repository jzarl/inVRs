set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/gmtl)

#install (DIRECTORY gmtl/
#		DESTINATION ${TARGET_INCLUDE_DIR}
#		REGEX "svn$" EXCLUDE)

install (FILES gmtl/AABox.h
		gmtl/AABoxOps.h
		gmtl/AxisAngle.h
		gmtl/AxisAngleOps.h
		gmtl/Comparitors.h
		gmtl/Config.h
		gmtl/Containment.h
		gmtl/Coord.h
		gmtl/CoordOps.h
		gmtl/Defines.h
		gmtl/EulerAngle.h
		gmtl/EulerAngleOps.h
		gmtl/Frustum.h
		gmtl/FrustumOps.h
		gmtl/Generate.h
		gmtl/Helpers.h
		gmtl/Intersection.h
		gmtl/LineSeg.h
		gmtl/LineSegOps.h
		gmtl/Math.h
		gmtl/Matrix.h
		gmtl/MatrixOps.h
		gmtl/OOBox.h
		gmtl/Output.h
		gmtl/Plane.h
		gmtl/PlaneOps.h
		gmtl/Point.h
		gmtl/Quat.h
		gmtl/QuatOps.h
		gmtl/Ray.h
		gmtl/RayOps.h
		gmtl/Sphere.h
		gmtl/SphereOps.h
		gmtl/Tri.h
		gmtl/TriOps.h
		gmtl/Vec.h
		gmtl/VecBase.h
		gmtl/VecExprMeta.h
		gmtl/VecOps.h
		gmtl/VecOpsMeta.h
		gmtl/Version.h
		gmtl/Xforms.h
		gmtl/gmtl.doxygen
		gmtl/gmtl.h
	DESTINATION ${TARGET_INCLUDE_DIR})

install (FILES gmtl/External/OpenSGConvert.h
	DESTINATION ${TARGET_INCLUDE_DIR}/External)
		
install (FILES gmtl/Fit/GaussPointsFit.h
	DESTINATION ${TARGET_INCLUDE_DIR}/Fit)

install (FILES gmtl/Misc/MatrixConvert.h
	DESTINATION ${TARGET_INCLUDE_DIR}/Misc)

install (FILES gmtl/Numerics/Eigen.h
	DESTINATION ${TARGET_INCLUDE_DIR}/Numerics)

install (FILES gmtl/Util/Assert.h
		gmtl/Util/Meta.h
		gmtl/Util/StaticAssert.h
	DESTINATION ${TARGET_INCLUDE_DIR}/Util)

