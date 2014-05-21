if (NOT OOPS_CONFIG_CMAKE_LOADED)

find_package(ODE)

include_directories (${ODE_INCLUDE_DIR})
include_directories ( "${CMAKE_CURRENT_BINARY_DIR}/include" )

set ( ODESOURCEPATH_JOINT_H "${ODE_SOURCE_DIR}/ode/src/joint.h" )
if ( NOT EXISTS "${ODE_SOURCE_DIR}/ode/src/joint.h" )
	message( FATAL_ERROR "Could not find correct value for ODESOURCEPATH_JOINT_H!" )
endif()
# 2012-08-27 ZaJ: quoting in cmake seems to have changed since the last time we used this:
#add_definitions (-DODESOURCEPATH_JOINT_H='\"${ODESOURCEPATH_JOINT_H}\"')
# -> add compiled header file
configure_file( "${CMAKE_CURRENT_LIST_DIR}/oops-include-ode-joint.h.in" "${CMAKE_CURRENT_BINARY_DIR}/include/oops/odeJoints/oops-include-ode-joint.h" @ONLY)

set (OOPS_CONFIG_CMAKE_LOADED ON)
endif (NOT OOPS_CONFIG_CMAKE_LOADED)

