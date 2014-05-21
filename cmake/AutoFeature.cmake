# - Declare an option with optional dependencies.
# If the dependencies are not met, the feature is disabled,
# otherwise the default value for the option is to be enabled.
#
# AUTOFEATURE( <feature> <feature_variable> <description> [QUIET] [EXPERIMENTAL]
#              [REQUIRED_PACKAGES <pkg1> .. <pkgN>]
#              [REQUIRED_CONDITIONS <cond1> .. <condN>] )
#  <feature>          - A name for the feature (used as label in the summary).
#  <feature_variable> - The name of the cache variable used by the feature.
#  <description>      - The description used for the cache variable.
#
# You can also automatically disable the feature is certain conditions are not met.
# One way to do this is by using REQUIRED_PACKAGES, followed by the packages that
# are needed by the feature. You can add additional arguments for the underlying
# find_package call by adding it to the package as follows:
# E.g. if the feature uses "FIND_PACKAGE(OpenSG REQUIRED COMPONENTS OSGBase)", you
# would use "OpenSG:COMPONENTS:OSGBase" (without the "REQUIRED"!) as required package.
# Another possibility for adding requirements is the REQUIRED_CONDITIONS parameter.
# You can pass any condition that is possible for an if statement, replacing spaces
# with colons (':').
#
# By default, any feature is enabled (if the dependencies are met). By adding the
# argument EXPERIMENTAL, you can disable features by default that are not yet stable
# enough.

# Copyright 2013 Johannes Zarl <johannes.zarl@jku.at>

function(AUTOFEATURE feature feature_var description)
	set(default ON)
	# swallow required args from ARGV:
	list(REMOVE_AT ARGV 0 1 2)
	# parse rest of parameters:
	set(arg_mode)
	set(REQUIRED_PACKAGES)
	set(REQUIRED_CONDITIONS)
	foreach(arg ${ARGV})
		if("REQUIRED_PACKAGES" STREQUAL "${arg}")
			set(arg_mode "MODE_REQUIRED_PACKAGES")
		elseif("REQUIRED_CONDITIONS" STREQUAL "${arg}")
			set(arg_mode "MODE_REQUIRED_CONDITIONS")
		elseif(QUIET STREQUAL "${arg}")
			set(arg_mode)
			set(AUTOFEATURE_QUIET TRUE)
		elseif(EXPERIMENTAL STREQUAL "${arg}")
			set(arg_mode)
			set(experimental true)
			set(default OFF)
		elseif("MODE_REQUIRED_PACKAGES" STREQUAL "${arg_mode}")
			list(APPEND REQUIRED_PACKAGES "${arg}")
		elseif("MODE_REQUIRED_CONDITIONS" STREQUAL "${arg_mode}")
			list(APPEND REQUIRED_CONDITIONS "${arg}")
		else()
			MESSAGE(AUTHOR_WARNING "AUTO_FEATURE(): unknown argument: ${arg}")
		endif()
	endforeach()
	if(REQUIRED_PACKAGES)
		list(REMOVE_DUPLICATES REQUIRED_PACKAGES)
		list(SORT REQUIRED_PACKAGES)
	endif()

	# check packages:
	set(missing_pkgs)
	set(required_pkgs)
	foreach(pkgdefraw ${REQUIRED_PACKAGES})
		# pkgdef may be a list like Qt4;COMPONENTS;QtCore
		# "unpack" list:
		string(REPLACE ":" ";" pkgdef "${pkgdefraw}")
		list(GET pkgdef 0 pkg)
		list(APPEND required_pkgs "${pkgdefraw}")
		find_package(${pkgdef} QUIET)
		string(TOUPPER "${pkg}" PKG)
		if(NOT ${pkg}_FOUND AND NOT ${PKG}_FOUND)
			list(APPEND missing_pkgs "${pkgdefraw}")
		endif()
	endforeach()

	#check conditions
	set(missing_conditions)
	foreach(cond ${REQUIRED_CONDITIONS})
		string(REPLACE ":" ";" expanded_cond "${cond}")
		#message("check: NOT(${expanded_cond})")
		if(${expanded_cond})
			# we want the else branch without negating the condition
		else()
			list(APPEND missing_conditions "${cond}")
		endif()
	endforeach()

	list(APPEND AUTOFEATURE_FEATURES "${feature}")
	list(SORT AUTOFEATURE_FEATURES)
	list(REMOVE_DUPLICATES AUTOFEATURE_FEATURES)
	set("AUTOFEATURE_FEATURES" "${AUTOFEATURE_FEATURES}" CACHE INTERNAL "List of features" FORCE)
	set("AUTOFEATURE_VAR2F_${feature_var}" "${feature}" CACHE INTERNAL "Map feature variable to feature" FORCE)
	set("AUTOFEATURE_F2VAR_${feature}" "${feature_var}" CACHE INTERNAL "Map feature to feature variable" FORCE)
	set("AUTOFEATURE_${feature}_REQUIRED_PACKAGES" "${required_pkgs}" CACHE INTERNAL "Dependencies for feature ${feature}" FORCE)
	set("AUTOFEATURE_${feature}_MISSING_PACKAGES" "${missing_pkgs}" CACHE INTERNAL "Missing dependencies for feature ${feature}" FORCE)
	set("AUTOFEATURE_${feature}_REQUIRED_CONDITIONS" "${REQUIRED_CONDITIONS}" CACHE INTERNAL "Required conditions for feature ${feature}" FORCE)
	set("AUTOFEATURE_${feature}_MISSING_CONDITIONS" "${missing_conditions}" CACHE INTERNAL "Missing conditions for feature ${feature}" FORCE)
	if(experimental)
		set("AUTOFEATURE_${feature}_EXPERIMENTAL" TRUE CACHE INTERNAL "Feature is experimental" FORCE)
	else()
		unset("AUTOFEATURE_${feature}_EXPERIMENTAL" CACHE)
	endif()
	if(missing_pkgs)
		set(reason "${reason} (Missing package(s): ${missing_pkgs})")
	endif()
	if(missing_conditions)
		set(reason "${reason} (Missing condition(s): ${missing_conditions})")
	endif()
	if(reason)
		if(NOT AUTOFEATURE_QUIET)
			message(STATUS "Disabling feature ${feature}${reason}")
		endif()
		set(${feature_var} OFF CACHE BOOL "${description}" FORCE)
	else()
		option(${feature_var} "${description}" ${default})
	endif()
endfunction(AUTOFEATURE)
