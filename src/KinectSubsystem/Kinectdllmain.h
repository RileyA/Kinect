#ifndef ORYXKINECTSUBSYSTEM_H
#define ORYXKINECTSUBSYSTEM_H

#include "Oryx.h"

#ifdef WIN32

	#define WIN32_LEAN_AND_MEAN

	#include <windows.h>

    #ifdef BUILD_KINECT_DLL
        #define ORYX_KINECT_EXPORT __declspec(dllexport)
    #else
        #define ORYX_KINECT_EXPORT __declspec(dllimport)
    #endif

#else
    #define ORYX_KINECT_EXPORT
#endif

#endif
