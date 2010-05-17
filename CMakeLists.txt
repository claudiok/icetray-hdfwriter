#
# $Id: CMakeLists.txt 58879 2009-10-15 12:40:18Z panknin $
#

i3_project(hdfwriter 
	PYTHON_DIR python
	DOCS_DIR resources/docs
)

# support HDF5 installations outside of ports
# this should really be done with an hdf5.cmake in I3_SITE_CMAKE_DIR
#
# TDS May 17, 2010: I just spent a couple hours detangling a user's
# build due to interactions between this and the I3_PORTS hdf5.  Can
# we leave this disabled by default?
option(WITH_SYSTEM_HDF5 "Try to find hdf5 on the system, don't use the ports version" OFF)
if (WITH_SYSTEM_HDF5)
  INCLUDE(standalone_hdf5.cmake)
endif()

IF(HDF5_FOUND)

  # add the path to the place where the hdf5 lib header files exist
  INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIR})
  ADD_DEFINITIONS(-DH5_USE_16_API)

  i3_add_library(hdfwriter
    private/hdfwriter/*.cxx
    private/hdfwriter/*.c

    USE_TOOLS log4cplus boost hdf5
    # add all the projects needed, in particular those needed by the converters
    USE_PROJECTS icetray dataclasses tableio)

  add_subdirectory(private/pybindings)

  # add the path to the place where the hdf5 libs exist
  #TARGET_LINK_LIBRARIES(hdf-writer ${HDF5_LIBRARIES})
  i3_test_executable(test 
    private/test/*.cxx
    USE_PROJECTS dataclasses icetray phys-services hdfwriter tableio)


ELSE(HDF5_FOUND)

  MESSAGE(STATUS "*** hdf5 library not found. 
   *** Ask your admin to install the hdf5 package from IceCube ports.")
  MESSAGE(STATUS "Skipping build of ${PROJECT_NAME}")

ENDIF(HDF5_FOUND)

