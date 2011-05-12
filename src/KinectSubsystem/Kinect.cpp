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
		:mDevice(device)
		,mContext(context)
		,mCurrentLED(LC_OFF)
		,mVideoEnabled(false)
		,mDepthRGBBuffer(0)
	{
		// zero out the tilt
		setTiltAngle(0);

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
	}
	//-----------------------------------------------------------------------

	Kinect::~Kinect()
	{
		setVideoEnabled(false);
		disableRGBDepth();
		freenect_close_device(mDevice);
	}
	//-----------------------------------------------------------------------
	
	void Kinect::setVideoEnabled(bool enabled)
	{
		if(enabled && !mVideoEnabled)
		{
			freenect_start_depth(mDevice);
			freenect_start_video(mDevice);
		}
		else if(!enabled && mVideoEnabled)
		{
			freenect_stop_video(mDevice);
			freenect_stop_depth(mDevice);
		}
	}
	//-----------------------------------------------------------------------

	void Kinect::setTiltAngle(Real degrees)
	{
		freenect_set_tilt_degs(mDevice, degrees);
	}
	//-----------------------------------------------------------------------
	
	Real Kinect::getTiltAngle()
	{
		return freenect_get_tilt_degs(freenect_get_tilt_state(mDevice));
	}
	//-----------------------------------------------------------------------
	
	void Kinect::setLED(LEDColor color)
	{
		mCurrentLED = color;
		freenect_set_led(mDevice, static_cast<freenect_led_options>(color));
	}
	//-----------------------------------------------------------------------

	Kinect::LEDColor Kinect::getLED()
	{
		return mCurrentLED;
	}
	//-----------------------------------------------------------------------

	void Kinect::enableRGBDepth()
	{
		if(!mDepthRGBBuffer)
			mDepthRGBBuffer = new byte[648*480*3];
	}
	//-----------------------------------------------------------------------

	void Kinect::disableRGBDepth()
	{
		if(mDepthRGBBuffer)
		{
			delete[] mDepthRGBBuffer;
			mDepthRGBBuffer = 0;
		}
	}
	//-----------------------------------------------------------------------

	byte* Kinect::getRGBDepth()
	{
		return mDepthRGBBuffer;
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
		if(mDepthRGBBuffer)
		{
			k_depth* depth = static_cast<k_depth*>(data);
			
			for(int i = 0; i< 640*480; ++i)
			{
				// simply constrain to 0-255 for now...
				byte intensity = static_cast<byte>((depth[i]/2047.f)*255.f);

				mDepthRGBBuffer[i*3+0] = intensity;
				mDepthRGBBuffer[i*3+1] = intensity;
				mDepthRGBBuffer[i*3+2] = intensity;
			}
		}
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
