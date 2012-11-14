#
# $Id$
#

i3_project(hdfwriter 
	PYTHON_DIR python
	DOCS_DIR resources/docs
)

IF(HDF5_FOUND)

  # add the path to the place where the hdf5 lib header files exist
  INCLUDE_DIRECTORIES(${HDF5_INCLUDE_DIR})
  ADD_DEFINITIONS(-DH5_USE_16_API)

  i3_add_library(hdfwriter
    private/hdfwriter/*.cxx
    private/hdfwriter/*.c

    USE_TOOLS boost hdf5
    # add all the projects needed, in particular those needed by the converters
    USE_PROJECTS icetray dataclasses tableio)

  add_subdirectory(private/pybindings)

  # add the path to the place where the hdf5 libs exist
  #TARGET_LINK_LIBRARIES(hdf-writer ${HDF5_LIBRARIES})
  i3_test_executable(test 
    private/test/*.cxx
    USE_PROJECTS dataclasses icetray phys-services hdfwriter tableio)

  # Link manipulation scripts into $I3_BUILD/bin for ease of use.
  execute_process(COMMAND ln -fsn ${CMAKE_CURRENT_SOURCE_DIR}/resources/scripts/merge.py ${EXECUTABLE_OUTPUT_PATH}/hdfwriter-merge)
  execute_process(COMMAND ln -fsn ${CMAKE_CURRENT_SOURCE_DIR}/resources/scripts/mix.py ${EXECUTABLE_OUTPUT_PATH}/hdfwriter-mix)

ELSE(HDF5_FOUND)

  colormsg(CYAN   "+-- hdf5 library not found, skipping build of ${PROJECT_NAME}") 
  colormsg(YELLOW "*** Ask your admin to install the hdf5 package from IceCube ports or 
       run `cmake -DSYSTEM_PACKAGES` to use the HDF5 libraries and headers
       packaged with your system.")

ENDIF(HDF5_FOUND)

