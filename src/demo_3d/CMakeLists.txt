# Build file for the Kinect Subsystem
set(SUBSYSTEMS
	OgreSubsystem
	OISSubsystem
	KinectSubsystem
	)

set(SOURCE_FILES
	kinect_demo_3d.cpp
	FPSCamera.cpp
	PlayState.cpp
	)

set(HEADER_FILES
	FPSCamera.h
	kinect_demo.h
	PlayState.h
	)

add_executable(demo_3d ${SOURCE_FILES} ${HEADER_FILES} )
set_target_properties(demo_3d PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (demo_3d ${ORYX_LIB} ${SUBSYSTEMS})
