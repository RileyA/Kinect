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

# freenect
set(FREENECT_INCLUDE_DIR "/usr/include" CACHE STRING "Where to look for freenect includes.")
set(FREENECT_LIB_DIR "/usr/lib" CACHE STRING "Where to look for freenect libs.")

# Oryx
set(ORYX_INCLUDE_DIR "/usr/include" CACHE STRING "Where to look for Oryx includes.")
set(ORYX_LIB_DIR "/usr/lib" CACHE STRING "Where to look for Oryx libs.")

# common includes
include_directories("${ORYX_INCLUDE_DIR}")
include_directories("src")

# lib stuff
set(ORYX_LIB ${ORYX_LIB_DIR}/${LIB_PREFIX}OryxEngine${LIB_POSTFIX})
set(FREENECT_LIB ${FREENECT_LIB_DIR}/${LIB_PREFIX}freenect${LIB_POSTFIX})

# add the subsystem itself
add_subdirectory(src/KinectSubsystem)

# the simple SDL demo
add_subdirectory(src/demo)

# point cloud demo
add_subdirectory(src/demo_3d)

# painter test thing
#add_subdirectory(src/demo)
