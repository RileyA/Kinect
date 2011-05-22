# Build file for the Kinect Subsystem
include_directories("${FREENECT_INCLUDE_DIR}")

set(SOURCE_FILES
	Kinect.cpp
	KinectSubsystem.cpp
	Kinectdllmain.cpp
	)

set(HEADER_FILES
	Kinect.h
	KinectSubsystem.h
	Kinectdllmain.h
	)

add_library(KinectSubsystem SHARED ${SOURCE_FILES} ${HEADER_FILES} )
set_target_properties(KinectSubsystem PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (KinectSubsystem ${ORYX_LIB} freenect boost_thread-mt)
