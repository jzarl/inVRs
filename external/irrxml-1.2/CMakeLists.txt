include_directories (src)

set (TARGET_INCLUDE_DIR ${INVRS_TARGET_INCLUDE_DIR}/irrXML)
set (TARGET_LIB_DIR ${INVRS_TARGET_LIB_DIR})
set (TARGET_BIN_DIR ${INVRS_TARGET_BIN_DIR})

add_library (irrXML src/irrXML.cpp)

#install (DIRECTORY src/
#		DESTINATION ${TARGET_INCLUDE_DIR}
#		PATTERN "*.cpp" EXCLUDE
#		REGEX "svn$" EXCLUDE)

install (FILES src/CXMLReaderImpl.h
        src/fast_atof.h
        src/heapsort.h
        src/irrArray.h
        src/irrString.h
        src/irrTypes.h
        src/irrXML.h
    DESTINATION ${TARGET_INCLUDE_DIR})

install(
	TARGETS irrXML
	ARCHIVE DESTINATION ${TARGET_LIB_DIR}
	LIBRARY DESTINATION ${TARGET_LIB_DIR}
	RUNTIME DESTINATION ${TARGET_BIN_DIR}
)

# additional exports (for inVRS):
set ( INVRS_EXPORT_irrXML_INCLUDE_DIRS )
set ( INVRS_EXPORT_irrXML_LIBRARY_DIRS )
set ( INVRS_EXPORT_irrXML_LIBRARIES irrXML)
set ( INVRS_EXPORT_irrXML_DEFINITIONS )
INVRS_ADD_EXPORTS(irrXML)
