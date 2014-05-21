# VERSION 1.2
#############
# CHANGES from 1.1 to 1.2:
# - updated MARK_AS_ADVANCED to only display values in GUI if something was not found (correctly)
# CHANGES from 1.0 to 1.1:
# - added MARK_AS_ADVANCED to avoid listing of variables in Simple View (in GUI)
# - renamed CHECK_XXX variables to XXX_CHECK to collect them in one group in Grouped View (in GUI)

if (NOT FIND_HELPER_MACROS)

# This macro provides commands for finding include directories and libraries

MACRO (FIND_INCLUDE_DIR_MACRO XXX_NAME XXX_CAPITAL_NAME XXX_HEADER_FILE XXX_PREDEFINED_PATHS XXX_PREDEFINED_SUFFIXES)

 set (XXX_INCLUDE_DIR ${XXX_CAPITAL_NAME}_INCLUDE_DIR)
 set (CHECK_XXX_INCLUDE_DIR ${XXX_CAPITAL_NAME}_CHECK_INCLUDE_DIR)

 message (STATUS "Check for ${XXX_NAME} include directory: ${${XXX_INCLUDE_DIR}}")
 find_path (${XXX_INCLUDE_DIR} ${XXX_HEADER_FILE}
 	PATHS ${${XXX_INCLUDE_DIR}} ${${XXX_PREDEFINED_PATHS}}
	PATH_SUFFIXES ${${XXX_PREDEFINED_SUFFIXES}})
 find_file (${CHECK_XXX_INCLUDE_DIR} ${XXX_HEADER_FILE}
 	PATHS ${${XXX_INCLUDE_DIR}})

 MARK_AS_ADVANCED(${CHECK_XXX_INCLUDE_DIR})
 if (NOT ${XXX_INCLUDE_DIR})
	 MARK_AS_ADVANCED(CLEAR ${XXX_INCLUDE_DIR})
	 message (FATAL_ERROR "Unable to find ${XXX_NAME} include directory!\n"
                "The ${XXX_NAME} include directory must contain the file ${XXX_HEADER_FILE}, e.g.:\n"
                "\t<${XXX_INCLUDE_DIR}>/${XXX_HEADER_FILE}\n"
	 	"Please pass the path to the ${XXX_NAME} includes to the cmake command, e.g.:\n"
		"\tcmake -D${XXX_INCLUDE_DIR}=/path/to/${XXX_NAME}/includes\n"
		"or define the path in the user.cmake file!")
 elseif (NOT ${CHECK_XXX_INCLUDE_DIR})
	MARK_AS_ADVANCED(CLEAR ${XXX_INCLUDE_DIR})
        message (FATAL_ERROR "Invalid ${XXX_NAME} include directory passed!\n"
                "The ${XXX_NAME} include directory must contain the file ${XXX_HEADER_FILE}, e.g.:\n"
                "\t<${XXX_INCLUDE_DIR}>/${XXX_HEADER_FILE}")
 else (NOT ${XXX_INCLUDE_DIR})
	MARK_AS_ADVANCED(FORCE ${XXX_INCLUDE_DIR})
	message (STATUS "Check for ${XXX_NAME} include directory: ${${XXX_INCLUDE_DIR}}")
 endif (NOT ${XXX_INCLUDE_DIR})
ENDMACRO (FIND_INCLUDE_DIR_MACRO)


MACRO (FIND_LIBRARY_MACRO XXX_NAME XXX_CAPITAL_NAME XXX_LIBRARY_NAME XXX_PREDEFINED_PATHS XXX_PREDEFINED_SUFFIXES)
 
 set (XXX_LIBRARY_FILE ${XXX_CAPITAL_NAME}_${XXX_LIBRARY_NAME}_LIBRARY)
 set (XXX_LIBRARY_DIR ${XXX_CAPITAL_NAME}_LIBRARY_DIR)
 set (XXX_LIBRARY_SPECIFIC_DIR ${XXX_CAPITAL_NAME}_${XXX_LIBRARY_NAME}_LIBRARY_DIR)
# set (CHECK_XXX_LIBRARY_FILE ${XXX_CAPITAL_NAME}_CHECK_${XXX_LIBRARY_NAME}_LIBRARY)


 message (STATUS "Check for ${XXX_NAME} - ${XXX_LIBRARY_NAME} library: ${${XXX_LIBRARY_FILE}}")
 find_library (${XXX_LIBRARY_FILE} ${XXX_LIBRARY_NAME}
 	PATHS ${${XXX_PREDEFINED_PATHS}} ${${XXX_LIBRARY_SPECIFIC_DIR}} ${${XXX_LIBRARY_DIR}}
	PATH_SUFFIXES ${${XXX_PREDEFINED_SUFFIXES}})
# find_file (${CHECK_XXX_LIBRARY_FILE} ${${XXX_LIBRARY_FILE}}
# 	PATHS /)

# MARK_AS_ADVANCED(${CHECK_XXX_LIBRARY_FILE})
 if (NOT ${XXX_LIBRARY_FILE})
	set (${XXX_LIBRARY_DIR} ${${XXX_LIBRARY_DIR}} CACHE PATH "Path to the ${XXX_NAME} libraries" FORCE)
	set (${XXX_LIBRARY_SPECIFIC_DIR} ${${XXX_LIBRARY_SPECIFIC_DIR}} CACHE PATH "Path to the ${XXX_LIBRARY_DIR} library" FORCE)
	MARK_AS_ADVANCED(CLEAR ${XXX_LIBRARY_FILE} ${XXX_LIBRARY_DIR} ${XXX_LIBRARY_SPECIFIC_DIR})
        message (FATAL_ERROR "Unable to find ${XXX_NAME} - ${XXX_LIBRARY_NAME} library!\n"
		"OPTION 1: You can set the url to the library directly by setting the variable\n"
		"\t${XXX_LIBRARY_FILE}\n"
		"OPTION 2: You can set the general search path for all ${XXX_NAME} libraries by\n"
		"\tsetting the variable ${XXX_LIBRARY_DIR}\n"
		"OPTION 3: You can set the path to the specific ${XXX_LIBRARY_NAME} library of the\n"
		"\t${XXX_NAME} package by setting the variable ${XXX_LIBRARY_SPECIFIC_DIR}\n")
# elseif (NOT ${CHECK_XXX_LIBRARY_FILE})
#        set (${XXX_LIBRARY_DIR} ${${XXX_LIBRARY_DIR}} CACHE PATH "Path to the ${XXX_NAME} libraries" FORCE)
#        set (${XXX_LIBRARY_SPECIFIC_DIR} ${${XXX_LIBRARY_SPECIFIC_DIR}} CACHE PATH "Path to the ${XXX_LIBRARY_DIR} library" FORCE)
#        MARK_AS_ADVANCED(CLEAR ${XXX_LIBRARY_FILE} ${XXX_LIBRARY_DIR} ${XXX_LIBRARY_SPECIFIC_DIR})
#        message (FATAL_ERROR "Invalid library ${XXX_LIBRARY_NAME} set!\n"
#		"The passed library file could not be found!\n")
 else (NOT ${XXX_LIBRARY_FILE})
	MARK_AS_ADVANCED(FORCE ${XXX_LIBRARY_FILE} ${XXX_LIBRARY_DIR} ${XXX_LIBRARY_SPECIFIC_DIR})
 	message (STATUS "Check for ${XXX_NAME} - ${XXX_LIBRARY_NAME} library: ${${XXX_LIBRARY_FILE}}")
 endif (NOT ${XXX_LIBRARY_FILE})
ENDMACRO (FIND_LIBRARY_MACRO)


MACRO (FIND_DIR_MACRO XXX_NAME XXX_DIR XXX_FILE XXX_PREDEFINED_PATHS XXX_PREDEFINED_SUFFIXES)

 set (CHECK_XXX_DIR ${XXX_DIR}_CHECK)

 message (STATUS "Check for ${XXX_NAME} directory: ${${XXX_DIR}}")
 find_path (${XXX_DIR} ${XXX_FILE}
        PATHS ${${XXX_DIR}} ${${XXX_PREDEFINED_PATHS}}
        PATH_SUFFIXES ${${XXX_PREDEFINED_SUFFIXES}})
 find_file (${CHECK_XXX_DIR} ${XXX_FILE}
        PATHS ${${XXX_DIR}})

 MARK_AS_ADVANCED(${CHECK_XXX_DIR})
 if (NOT ${XXX_DIR})
	 MARK_AS_ADVANCED(CLEAR ${XXX_DIR})
         message (FATAL_ERROR "Unable to find ${XXX_NAME} directory!\n"
                "The ${XXX_NAME} directory must contain the file ${XXX_FILE}, e.g.:\n"
                "\t<${XXX_DIR}>/${XXX_FILE}\n"
                "Please pass the path to the ${XXX_NAME} directory to the cmake command, e.g.:\n"
                "\tcmake -D${XXX_DIR}=/path/to/${XXX_NAME}/includes\n"
                "or define the path in the user.cmake file!")
 elseif (NOT ${CHECK_XXX_DIR})
 	MARK_AS_ADVANCED(CLEAR ${XXX_DIR})
        message (FATAL_ERROR "Invalid ${XXX_NAME} directory passed!\n"
                "The ${XXX_NAME} directory must contain the file ${XXX_FILE}, e.g.:\n"
                "\t<${XXX_DIR}>/${XXX_FILE}")
 else (NOT ${XXX_DIR})
	MARK_AS_ADVANCED(FORCE ${XXX_DIR})
        message (STATUS "Check for ${XXX_NAME} directory: ${${XXX_DIR}}")
 endif (NOT ${XXX_DIR})
ENDMACRO (FIND_DIR_MACRO)

SET (FIND_HELPER_MACROS ON)
ENDIF (NOT FIND_HELPER_MACROS)

