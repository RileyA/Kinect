//---------------------------------------------------------------------------
//(C) Copyright Riley Adams 2010

//This file is part of Oryx Engine.

// Oryx Engine is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// Oryx Engine is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with Oryx Engine. If not, see <http://www.gnu.org/licenses/>.
//---------------------------------------------------------------------------

#ifndef ORYX_KINECT_SUBSYSTEM_H
#define ORYX_KINECT_SUBSYSTEM_H

#include "Oryx.h"
#include "Kinectdllmain.h"
#include "OryxEngineSubsystem.h"

typedef struct _freenect_device freenect_device;
typedef struct _freenect_context freenect_context;

namespace Oryx
{
	void depthCallback(freenect_device *dev, void *v_depth, uint32_t timestamp);
	void colorCallback(freenect_device *dev, void *rgb, uint32_t timestamp);

    class ORYX_KINECT_EXPORT KinectSubsystem : public EngineSubsystem
    {
    public:

        KinectSubsystem();
        ~KinectSubsystem();

        virtual void _init();
        virtual void _deinit();
        virtual void _update(Real delta);
        virtual void _endState();

        virtual String getName();

    protected:

		void depthCallback(freenect_device* device, void *data, uint32_t time);
		void colorCallback(freenect_device* device, void *data, uint32_t time);

        bool mInitialized;
		freenect_context* mContext;

		// stuff for the device (should be split into its own class at some point
		freenect_device* mDevice;

		// vidya buffers
		byte mDepthBuffer_0[640][480][3];
		byte mDepthBuffer_1[640][480][3];
		byte mColorBuffer_0[640][480][3];
		byte mColorBuffer_1[640][480][3];

    };
}

#endif
