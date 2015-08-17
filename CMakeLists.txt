#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(msc)

# Find Geant4
if(NOT DEFINED Geant4_DIR)
	set(Geant4_DIR $ENV{G4INSTALL}/build)
endif(NOT DEFINED Geant4_DIR)
find_package(Geant4 REQUIRED COMPONENTS gdml qt ui_all vis_all)
execute_process(COMMAND geant4-config --version
                OUTPUT_VARIABLE GEANT4_VERSION
                OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Found Geant4 ${GEANT4_VERSION}")

# Add the project include directory and put all source files in sources
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
if(GEANT4_VERSION VERSION_LESS "10.0.0")
       message(STATUS "Geant4 is < 10.0.0: skip building custom physics processes")
       file(GLOB sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
       file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh)
else(GEANT4_VERSION VERSION_LESS "10.0.0")
       message(STATUS "Geant4 > 10.0.0: include building custom physics processes")
       file(GLOB_RECURSE sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
       file(GLOB_RECURSE headers ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hh)
endif(GEANT4_VERSION VERSION_LESS "10.0.0")

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Load ROOT and setup include directory
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(msc msc.cc ${sources} ${headers})
target_link_libraries(msc ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build msc. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(MSC_SCRIPTS
  macros/icons.mac
  macros/gui.mac
  macros/run.png
  macros/init.mac
  macros/init_vis.mac
  macros/run1.mac
  macros/run2.mac
  macros/vis.mac
  )

foreach(_script ${MSC_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS msc DESTINATION bin)
