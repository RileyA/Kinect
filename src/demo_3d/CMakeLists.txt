# Build file for the Kinect Subsystem
set(SUBSYSTEMS
	${OGRE_LIB}
	${KINECT_LIB}
	${OIS_LIB}
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

#install(TARGETS demo_3d
#		RUNTIME DESTINATION ${INSTALL_RUNTIMES_TO}/kinect
#		LIBRARY DESTINATION ${INSTALL_LIBS_TO}
#		ARCHIVE DESTINATION ${INSTALL_LIBS_TO})
