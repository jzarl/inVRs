if (NOT GMTL_INCLUDE_DIR)

message (STATUS "Check for GMTL include directory: ${GMTL_INCLUDE_DIR}")

find_path (GMTL_INCLUDE_DIR gmtl/gmtl.h
        PATHS ${GMTL_INCLUDE_DIR} ${INVRS_SOURCE_DIR}/external
	PATH_SUFFIXES gmtl-0.6.1)

find_file (GMTL_CHECK_DIR gmtl/gmtl.h
        PATHS ${GMTL_INCLUDE_DIR})

if (NOT GMTL_INCLUDE_DIR)
        message (FATAL_ERROR "Unable to find GMTL include directory!\n"
                "OPTION A: If you installed GMTL (v0.5.4) please pass the path to the GMTL\n"
                "          directory to the cmake command, e.g.:\n"
                "          cmake <path_to_inVRs> -DGMTL_INCLUDE_DIR=/path/to/gmtl\n"
                "OPTION B: You can download the sources from: \n"
                "          http://sourceforge.net/projects/ggt - the recommended version is 0.5.4\n"
                "          Unpack the sources into following folder for automatic detection:\n"
                "          ${INVRS_SOURCE_DIR}/external\n")
elseif (NOT GMTL_CHECK_DIR)
        message (FATAL_ERROR "Invalid GMTL include directory passed!\n"
                "The GMTL include directory must contain the subfolder gmtl, e.g.:\n"
                "\t/<GMTL_INCLUDE_DIR>/gmtl/gmtl.h")
else (NOT GMTL_INCLUDE_DIR)
        message (STATUS "Check for GMTL include directory: ${GMTL_INCLUDE_DIR} - done")
endif (NOT GMTL_INCLUDE_DIR)

endif (NOT GMTL_INCLUDE_DIR)

