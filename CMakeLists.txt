# Copyright (c) 2011-2015:  G-CSC, Goethe University Frankfurt
# Authors: Sebastian Reiter, Arne Naegel
# 
# This file is part of UG4.
# 
# UG4 is free software: you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License version 3 (as published by the
# Free Software Foundation) with the following additional attribution
# requirements (according to LGPL/GPL v3 §7):
# 
# (1) The following notice must be displayed in the Appropriate Legal Notices
# of covered and combined works: "Based on UG4 (www.ug4.org/license)".
# 
# (2) The following notice must be displayed at a prominent place in the
# terminal output of covered works: "Based on UG4 (www.ug4.org/license)".
# 
# (3) The following bibliography is recommended for citation and must be
# preserved in all covered files:
# "Reiter, S., Vogel, A., Heppner, I., Rupp, M., and Wittum, G. A massively
#   parallel geometric multigrid solver on hierarchically distributed grids.
#   Computing and visualization in science 16, 4 (2013), 151-164"
# "Vogel, A., Reiter, S., Rupp, M., Nägel, A., and Wittum, G. UG4 -- a novel
#   flexible software system for simulating pde based models on high performance
#   computers. Computing and visualization in science 16, 4 (2013), 165-179"
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.

################################################################################
# TiffImageData
#
# This script shows how easy it is to add a build script for your plugins.
# Simply specify the plugins name and a list of your source files.
################################################################################

# Name of your plugin and sources.
set(pluginName	IJKRasterData)
set(SOURCES		src/ijk_plugin.cpp)

# Tiff-plugin.
set(SOURCES ${SOURCES}
	src/neura2/c3dimage.cpp
	src/neura2/c3dimage_tiff.cpp)

	
################################################################################
# The code below doesn't have to be changed (usually)
################################################################################

cmake_minimum_required(VERSION 2.8.12...3.20)
project(UG_PLUGIN_${pluginName})

# Include the definitions and dependencies for ug-plugins.
if(NOT DEFINED UG_ROOT_CMAKE_PATH OR UG_ROOT_CMAKE_PATH STREQUAL "")
    # message(STATUS "UG_ROOT_CMAKE_PATH is not set or is empty. Must provide a value.")
    include(../../ugcore/cmake/ug_plugin_includes.cmake) 
else()
    include(${UG_ROOT_CMAKE_PATH}/ug_plugin_includes.cmake) 
endif()



# get_libug4_from_pip
# return_value1: location of library in current pip directory.
function(get_libug4_from_pip_new RETURN_VAR)
	SET(UG4_STATIC_LIB_BASENAME ug4_s)
	#if(WIN32)
    # 	set(UG4_STATIC_LIB_NAME "${LIB_BASENAME}.lib")
	#else()
    #	set(UG4_STATIC_LIB_NAME "lib${LIB_BASENAME}.a")
	#endif()
	
	# Extract possible location of pip-package
	execute_process(
    		#COMMAND pip show ug4py-base
			COMMAND python3 -m pip show ug4py-base
    		OUTPUT_VARIABLE PIP_SHOW_OUTPUT
    		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	# Extract the "Location" line & remove the ' prefix to get the actual path
	set(UG4PY_BASE_PACKAGE_PATH "")
	string(COMPARE EQUAL "${PIP_SHOW_OUTPUT}" "" ISEMPTY)
	if (NOT ISEMPTY)
		string(REGEX MATCH "Location: .+" LOCATION_LINE "${PIP_SHOW_OUTPUT}")
		string(REPLACE "Location: " "" UG4PY_BASE_PACKAGE_PATH "${LOCATION_LINE}")
		string(REGEX MATCH "^[^\n\r]*" UG4PY_BASE_PACKAGE_PATH "${UG4PY_BASE_PACKAGE_PATH}") # remove lines.
	endif()
	message(STATUS "Package path hint: '${UG4PY_BASE_PACKAGE_PATH}'")

	# Find library.
	find_library(PYUG4_STATIC_LIBRARY
    			NAMES ${UG4_STATIC_LIB_BASENAME} lib${UG4_STATIC_LIB_BASENAME}
    			PATHS ${UG4PY_BASE_PACKAGE_PATH}/lib ${UG4PY_BASE_PACKAGE_PATH}/lib64)
	
	message(STATUS "Library ${UG4_STATIC_LIB_BASENAME} found: ${PYUG4_STATIC_LIBRARY}")
	
	set(${RETURN_VAR} "${PYUG4_STATIC_LIBRARY}" PARENT_SCOPE) 
endfunction()






# include libTIFF module 
if(INTERNAL_LIB_TIFF)
	# External libtiff from OS.
	find_package(TIFF REQUIRED)
	include_directories(${TIFF_INCLUDE_DIR})
	MESSAGE (STATUS "TIFF-lib: ${TIFF_LIBRARIES}")
	MESSAGE (STATUS "TIFF-inc: ${TIFF_INCLUDE_DIR}")
	MESSAGE (STATUS "TIFF-pybind11: ${USE_PYBIND11}")
else(INTERNAL_LIB_TIFF)
	# Internal libtiff.
	# Building own libtiff..
	message(STATUS, "Building LibTiff internally.")
	
	if(APPLE)
		# TODO: Per default, macos-13 builds for 10.9, macos-14 for 10.10. 
 		# But libs are taken from build OS...
 		# May be avoided by setting MACOSX_DEPLOYMENT_TARGET?
		set(zlib OFF CACHE BOOL "zlib" FORCE)
		set(libdeflate OFF CACHE BOOL "libdeflate" FORCE)
		set(webp OFF CACHE BOOL "libdeflate" FORCE)
		set(jpeg OFF CACHE BOOL "libjpeg" FORCE)
		set(old-jpeg OFF CACHE BOOL "libdeflate" FORCE)
		set(jpeg12 OFF CACHE BOOL "libjpeg12" FORCE)
 		set(zstd OFF CACHE BOOL "libzstd" FORCE)
  		set(lzma OFF CACHE BOOL "libdeflate" FORCE)
	endif(APPLE)
	
	set(tiff-tools OFF CACHE BOOL "build TIFF tools" FORCE)
	set(tiff-tests OFF CACHE BOOL "build TIFF tests" FORCE)
	set(tiff-contrib OFF CACHE BOOL "build TIFF contrib" FORCE)
	set(tiff-docs OFF CACHE BOOL "build TIFF documentation" FORCE)
	
	# Build form subdir
	add_subdirectory(libtiff)
	
endif(INTERNAL_LIB_TIFF)





set(TIFF_LIBRARY "tiff")
set(TIFF_INCLUDE_DIR "libtiff/libtiff")




################################################################################
# Classic binding (static or dynamic plugin).
################################################################################
if(NOT USE_PYBIND11)
	MESSAGE (STATUS "USE_PYBIND11 = OFF")
	if(buildEmbeddedPlugins)
 		MESSAGE (STATUS "buildEmbeddedPlugins = ON")
		# add the sources to ug4's sources
		EXPORTSOURCES(${CMAKE_CURRENT_SOURCE_DIR} ${SOURCES})
	else(buildEmbeddedPlugins)
 		MESSAGE (STATUS "buildEmbeddedPlugins = OFF")
		# create a shared library from the sources and link it against ug4.
		add_library(${pluginName} SHARED ${SOURCES})
		target_link_libraries (${pluginName} ug4 ${TIFF_LIBRARY})
	endif(buildEmbeddedPlugins)
endif(NOT USE_PYBIND11)

################################################################################
# Python binding (static plugin, dynamic python interface).
################################################################################
if(USE_PYBIND11)
	MESSAGE (STATUS "USE_PYBIND11 = ON")
	
 	# Static plugin.
	include_directories(${TIFF_INCLUDE_DIR})
	MESSAGE (STATUS "TIFF-inc: ${TIFF_INCLUDE_DIR}")
	
	###############################################
	# 0) Let's try to locate ug_4 static library....
	###############################################
	
	set(UG4_STATIC_LIBRARY ug4_s)
	# if (BUILD_WITH_PIP)
		get_libug4_from_pip_new(PYUG4_STATIC_LIBRARY)
		message(STATUS "Library found: ${PYUG4_STATIC_LIBRARY}")
		set(UG4_STATIC_LIBRARY ${PYUG4_STATIC_LIBRARY})
	# endif()
	message(STATUS "Using libug4_s: ${UG4_STATIC_LIBRARY}")
	
	###############################################
 	# 1) Create (static) library for plugin.
	###############################################
	add_library(IJKDataPlugin STATIC ${SOURCES})
	target_include_directories (IJKDataPlugin PUBLIC ${TIFF_INCLUDE_DIR} )
	target_include_directories (IJKDataPlugin PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/libtiff/libtiff )

	###############################################
 	# 2) Create (dynamic) python library module.
 	# First argumentin 'python_add_library' must match module name in PYBIND11_MODULE call!
	###############################################
	SET(myPluginSources src/ijk_plugin_pybind.cpp)
	python_add_library(pyijkdata MODULE ${myPluginSources} ${SOURCES} WITH_SOABI)
	target_link_directories (pyijkdata PRIVATE ${UG4PY_BASE_PACKAGE_PATH})
	target_link_libraries (pyijkdata PRIVATE IJKDataPlugin ${UG4_STATIC_LIBRARY} ${TIFF_LIBRARY})
	target_include_directories (pyijkdata PUBLIC ${TIFF_INCLUDE_DIR} )
	set_target_properties(pyijkdata PROPERTIES INSTALL_RPATH "$ORIGIN/..:$ORIGIN/../../../lib")
	install(TARGETS pyijkdata LIBRARY DESTINATION ug4py COMPONENT pymodules)

endif(USE_PYBIND11)
