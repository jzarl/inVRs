############################################################
# CMake script for creating tarballs for distribution
###
# Author: Johannes Zarl
###
# Usage: cmake -P tools/scripts/create-dist-tarball.cmake
#
# This script takes care of special needs during svn export,
# such as preserving the svn revision for libraries such as
# ufo.
############################################################
message( "NO_DELETE_EXPORT_TREE: ${NO_DELETE_EXPORT_TREE}" )
message( "NO_TARBALL: ${NO_TARBALL}")

## ensure correct working directory:
if ( NOT EXISTS "VERSION" )
	message( FATAL_ERROR "Could not find file \"VERSION\"!\nPlease ensure that this script is executed inside the base directory of the repository...")
endif()

############################################################
# Gather information
############################################################
file( STRINGS VERSION INVRS_VERSION LIMIT_COUNT 1)

find_package( Subversion REQUIRED QUIET )
Subversion_WC_INFO(. SVN)
# current revision and last-changed revision should be the same in the repository root:
IF ( NOT "${SVN_WC_REVISION}" STREQUAL "${SVN_WC_LAST_CHANGED_REV}" )
	message( STATUS "Assert failed: current revision == last changed revision")
	message( FATAL_ERROR "values: ${SVN_WC_REVISION} == ${SVN_WC_LAST_CHANGED_REV}")
endif()
set(REV ${SVN_WC_LAST_CHANGED_REV})

# extract date/time from last-changed-date:
string( REGEX MATCH "[0-9][0-9][0-9][0-9]-[0-9][0-9]-[0-9][0-9]" REV_DATE "${SVN_WC_LAST_CHANGED_DATE}" )
string( REGEX MATCH "[0-2][0-9]:[0-5][0-9]" REV_TIME "${SVN_WC_LAST_CHANGED_DATE}" )
string( REPLACE ":" "" REV_TIME ${REV_TIME} )

set(ARCHIVE_NAME inVRs_${INVRS_VERSION}-${REV_DATE}_${REV_TIME}-r${REV} )
message( STATUS "------------------------------------------------------------" )
message( STATUS "Version information:" )
message( STATUS "inVRs version: ${INVRS_VERSION}" )
message( STATUS "Revision: ${SVN_WC_LAST_CHANGED_REV}" )
message( STATUS "Revision date: ${SVN_WC_LAST_CHANGED_DATE}" )
message( STATUS "Archive name: ${ARCHIVE_NAME}" )
message( STATUS "------------------------------------------------------------" )


############################################################
# Create tarball
############################################################
message( STATUS "Exporting repository: to directory ${ARCHIVE_NAME}" )
execute_process( 
	COMMAND ${Subversion_SVN_EXECUTABLE} export . ${ARCHIVE_NAME}
	RESULT_VARIABLE SVN_ERROR )
if ( NOT ${SVN_ERROR} EQUAL 0 )
	message( FATAL_ERROR "svn export failed!" )
endif()

if ( NOT NO_TARBALL )
	message( STATUS "Creating archive in .tar.bz2 format..." )
	execute_process(
		COMMAND ${CMAKE_COMMAND} -E tar cjf "${ARCHIVE_NAME}.tar.bz2" "${ARCHIVE_NAME}"
		RESULT_VARIABLE TAR_ERROR )
	if ( NOT ${TAR_ERROR} EQUAL 0 )
		message( FATAL_ERROR "Tarball creation failed!" )
	endif()
endif()


############################################################
# Cleanup
############################################################
if ( NOT NO_DELETE_EXPORT_TREE )
	message( STATUS "Removing exported source tree..." )
	file( REMOVE_RECURSE "${ARCHIVE_NAME}" )
else()
	message( STATUS "Not removing exported source tree." )
endif()
message( STATUS "SUCCESS." )
