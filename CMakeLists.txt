#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 2.6 FATAL_ERROR)
project(msc)

# Find Geant4
SITE_NAME(mySite)
if(${mySite} MATCHES "ifarm*")
   set(Geant4_DIR /lustre/expphy/work/hallc/qweak/ciprian/geant41001p02/geant4.10.01.p02/build)
   message( "On ifarm using custom geant4_dir: " ${Geant4_DIR})
   find_package(Geant4 REQUIRED COMPONENTS gdml qt ui_all vis_all)
   execute_process(COMMAND ${Geant4_DIR}/geant4-config --version
                   OUTPUT_VARIABLE GEANT4_VERSION
                   OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
   if(NOT DEFINED Geant4_DIR)
	set(Geant4_DIR $ENV{G4INSTALL}/build)
   endif(NOT DEFINED Geant4_DIR)
   find_package(Geant4 REQUIRED COMPONENTS gdml qt ui_all vis_all)
   execute_process(COMMAND geant4-config --version
                   OUTPUT_VARIABLE GEANT4_VERSION
                   OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

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
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)
message(STATUS "Geant4 include files come from: ${Geant4_USE_FILE}")

# Load ROOT and setup include directory
find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIR})

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(msc msc.cc ${sources} ${headers})
target_link_libraries(msc ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

add_executable(calcPMTDD CalcPEsPMTDD.cc ${sources} ${headers})
target_link_libraries(calcPMTDD ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

add_executable(anaTree anaTree.cc ${sources} ${headers})
target_link_libraries(anaTree ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

add_executable(anaStp analyzeWithSteps.cc ${sources} ${headers})
target_link_libraries(anaStp ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

add_executable(multipleAsymSteps multipleAsymSteps.cc)
target_link_libraries(multipleAsymSteps ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

add_executable(anaWght anaWght.cc)
target_link_libraries(anaWght ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

add_executable(anaTrk anaTrk.cc)
target_link_libraries(anaTrk ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

add_executable(rotateSpinTst rotateSpinTst.cc)
target_link_libraries(rotateSpinTst ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})

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
