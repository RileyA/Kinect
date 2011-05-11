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

		void depthCallback(freenect_device* device, void *data, uint32_t time);
		void colorCallback(freenect_device* device, void *data, uint32_t time);

		byte* getRawDepth();
		byte* getRawColor();
	
	protected:

		bool mInitialized;
		freenect_context* mContext;

		// stuff for the device (should be split into its own class at some point
		freenect_device* mDevice;

		// vidya buffers
		byte mDepthBuffer[640][480][3];
		byte mColorBuffer[640][480][3];

		uint16_t mGamma[2048];

	};
}

#endif
