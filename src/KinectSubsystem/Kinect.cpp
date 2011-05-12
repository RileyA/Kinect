#include "Kinect.h"
#include "Oryx.h"
#include "KinectSubsystem.h"
#include "OryxLogger.h"
#include "OryxEngine.h"
#include "OryxTimeManager.h"
#include "libfreenect.h"

namespace Oryx
{
	Real Kinect::mDepths[2048] = {};

	Kinect::Kinect(freenect_device* device, freenect_context* context)
		:mDevice(device),mContext(context),mCurrentLED(LC_OFF)
	{
		// zero out the tilt
		freenect_set_tilt_degs(mDevice, 0);

		// set callbacks
		freenect_set_depth_callback(mDevice, Oryx::depthCallback);
		freenect_set_video_callback(mDevice, Oryx::colorCallback);

		// choose video modes
		freenect_set_video_mode(mDevice, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, 
			FREENECT_VIDEO_RGB));
		freenect_set_depth_mode(mDevice, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, 
			FREENECT_DEPTH_11BIT));

		// set the buffers
		freenect_set_video_buffer(mDevice, mColorBuffer);
		freenect_set_depth_buffer(mDevice, mDepthBuffer);

		// and start it up TODO: split out startup and such into different functions
		freenect_start_depth(mDevice);
		freenect_start_video(mDevice);
	}
	//-----------------------------------------------------------------------

	Kinect::~Kinect()
	{
		freenect_stop_video(mDevice);
		freenect_stop_depth(mDevice);
		freenect_close_device(mDevice);
	}
	//-----------------------------------------------------------------------
	
	k_depth* Kinect::getRawDepth()
	{
		return &mDepthBuffer[0];
	}
	//-----------------------------------------------------------------------
	
	byte* Kinect::getRawColor()
	{
		return &mColorBuffer[0];
	}
	//-----------------------------------------------------------------------
	
	/* Based on a posting by Stéphane Magnenat on the OpenKinect
	 *		Google Group, approx depths are in meters */
	void Kinect::preprocessDepth()
	{
		for(int i = 0; i < 2048; ++i)
			mDepths[i] = 0.1236 * tan(i/2842.5 + 1.1863);
	}
	//-----------------------------------------------------------------------
	
	void Kinect::depthCallback(freenect_device* device, void *data, uint32_t time)
	{

	}
	//-----------------------------------------------------------------------

	void Kinect::colorCallback(freenect_device* device, void *data, uint32_t time)
	{

	}
	//-----------------------------------------------------------------------
	
	void depthCallback(freenect_device *dev, void *v_depth, uint32_t timestamp)
	{
		Engine::getPtr()->getSubsystem("KinectSubsystem")->
			castType<KinectSubsystem>()->getDevice(dev)->depthCallback(dev, v_depth, timestamp);
	}
	//-----------------------------------------------------------------------

	void colorCallback(freenect_device *dev, void *rgb, uint32_t timestamp)
	{
		Engine::getPtr()->getSubsystem("KinectSubsystem")->
			castType<KinectSubsystem>()->getDevice(dev)->colorCallback(dev, rgb, timestamp);
	}
	//-----------------------------------------------------------------------
}
