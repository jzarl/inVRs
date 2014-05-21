# for some reason we don't inherit the include directories:
#include_directories (${UFO_SOURCE_DIR}/ufo ${GMTL_INCLUDE_DIR})

################################################################################
# general prefix for test-names:
################################################################################

set (TEST_PREFIX "${TEST_PREFIX}UFO_" )

################################################################################
# define tests
################################################################################
set ( UFOTESTS
	01testplainconfig
	02testplugins
	03testpluginload
	04testpluginmultiload
	05testpluginload_bigflock
	06testbehaviours
	)

################################################################################
# add specific settings for some tests:
################################################################################

# 01testplainconfig uses a sampleconfigplain.txt:
set (UFO_01testplainconfig_PARAMETERS ${UFO_SOURCE_DIR}/sampleconfigplain.txt )
# 02testplugins bypasses the plugin system, directly linking to Behaviour/Steerable:
set ( UFO_02testplugins_SOURCES_ADD ${UFO_SOURCE_DIR}/ufoplugins/SampleBehaviour.cpp ${UFO_SOURCE_DIR}/ufoplugins/SampleSteerable.cpp )

################################################################################
# Create test targets:
################################################################################
foreach (test ${UFOTESTS})
	# add target for test
	add_executable ( ${test} ${test}.cpp ${UFO_${test}_SOURCES_ADD} )
	# make test dependant on libufo target
	target_link_libraries ( ${test} ufo )
	# if not set, set default config file as standard parameter:
	if ( NOT UFO_${test}_PARAMETERS )
		set ( UFO_${test}_PARAMETERS "${CMAKE_CURRENT_SOURCE_DIR}/${test}.cfg" )
	endif ( NOT UFO_${test}_PARAMETERS )
	# add test:
	add_test ( ${TEST_PREFIX}${test} ${test} ${UFO_${test}_PARAMETERS} )
endforeach()


################################################################################
# Add tests using existing targets:
################################################################################
# ufo should fail with a non-existing input file
add_test ( ${TEST_PREFIX}01testplainconfig_fail 01testplainconfig / )
set_tests_properties ( ${TEST_PREFIX}01testplainconfig_fail PROPERTIES WILL_FAIL true )
# ufo should accept a configuration without flocks or pilots:
add_test ( ${TEST_PREFIX}03testpluginload_empty 03testpluginload ${CMAKE_CURRENT_SOURCE_DIR}/03testpluginload_empty.cfg )
