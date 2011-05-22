# Build file for the Kinect Subsystem
set(SUBSYSTEMS
	${KINECT_LIB}
	${SDL_LIB}
	)

set(SOURCE_FILES
	kinect_demo.cpp
	)

set(HEADER_FILES
	kinect_demo.h
	)

add_executable(demo ${SOURCE_FILES} ${HEADER_FILES} )
set_target_properties(demo PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries (demo ${ORYX_LIB} ${SUBSYSTEMS})
