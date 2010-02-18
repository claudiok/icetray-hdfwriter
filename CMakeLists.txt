#
# $Id: CMakeLists.txt 58879 2009-10-15 12:40:18Z panknin $
#

i3_project(hdfwriter 
	PYTHON_DIR python
	DOCS_DIR resources/docs
)

# support HDF5 installations outside of ports
# this should really be done with an hdf5.cmake in I3_SITE_CMAKE_DIR
INCLUDE(standalone_hdf5.cmake)

IF(HDF5_FOUND)

  # add the path to the place where the hdf5 lib header files exist
  INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIR})

  i3_add_library(hdfwriter
    private/hdfwriter/*.cxx

    USE_TOOLS log4cplus boost hdf5
    # add all the projects needed, in particular those needed by the converters
    USE_PROJECTS icetray tableio)

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

