cmake_minimum_required (VERSION 2.6)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")

# Some lib stuff, probably unneeded, but I don't know cmake too well yet...
if (WIN32)
	set(LIB_PREFIX )
	set(LIB_POSTFIX .lib)
else ()
	set(LIB_PREFIX lib)
	set(LIB_POSTFIX .so)
endif ()

# Some options:
set(INSTALL_RUNTIMES_TO "/usr/bin" CACHE STRING "Where install executables.")
set(INSTALL_LIBS_TO "/usr/lib" CACHE STRING "Where to install libs.")

option(BUILD_2D_DEMO "Build the 2d raw output demo." TRUE)
option(BUILD_3D_DEMO "Build 3d point cloud demo." TRUE)

# freenect
set(FREENECT_INCLUDE_DIR "/usr/include" CACHE STRING "Where to look for freenect includes.")
set(FREENECT_LIB_DIR "/usr/lib" CACHE STRING "Where to look for freenect libs.")

# Oryx
set(ORYX_INCLUDE_DIR "/usr/include" CACHE STRING "Where to look for Oryx includes.")
set(ORYX_LIB_DIR "/usr/lib" CACHE STRING "Where to look for Oryx libs.")
set(ORYX_SUBSYSTEM_INCLUDE_DIR "/usr/include" CACHE STRING "Where to look for Oryx Subsystem includes.")
set(ORYX_SUBSYSTEM_LIB_DIR "/usr/lib" CACHE STRING "Where to look for Oryx Subsystems.")

# boost
set(BOOST_INCLUDE_DIR "/usr/include" CACHE STRING "Where to look for boost includes.")
set(BOOST_LIB_DIR "/usr/lib" CACHE STRING "Where to look for boost libs.")

# common includes
include_directories("${ORYX_INCLUDE_DIR}")
include_directories("${ORYX_SUBSYSTEM_INCLUDE_DIR}")
include_directories("src")

# lib stuff, ugly, but gets the job done for now...
set(ORYX_LIB ${ORYX_LIB_DIR}/${LIB_PREFIX}OryxEngine${LIB_POSTFIX})
# subsystems
set(OGRE_LIB ${ORYX_SUBSYSTEM_LIB_DIR}/${LIB_PREFIX}OgreSubsystem${LIB_POSTFIX})
set(SDL_LIB ${ORYX_SUBSYSTEM_LIB_DIR}/${LIB_PREFIX}SDLSubsystem${LIB_POSTFIX})
set(OIS_LIB ${ORYX_SUBSYSTEM_LIB_DIR}/${LIB_PREFIX}OISSubsystem${LIB_POSTFIX})
set(KINECT_LIB ${ORYX_SUBSYSTEM_LIB_DIR}/${LIB_PREFIX}KinectSubsystem${LIB_POSTFIX})
# freenect and boost
set(FREENECT_LIB ${FREENECT_LIB_DIR}/${LIB_PREFIX}freenect${LIB_POSTFIX})
set(BOOST_THREAD_LIB ${BOOST_LIB_DIR}/${LIB_PREFIX}boost_thread-mt${LIB_POSTFIX})

# add the subsystem itself
add_subdirectory(src/KinectSubsystem)

if (BUILD_2D_DEMO)
	# the simple SDL demo
	add_subdirectory(src/demo)
endif ()

if (BUILD_3D_DEMO)
	# point cloud demo
	add_subdirectory(src/demo_3d)
endif ()

# painter test thing
#add_subdirectory(src/painter)

