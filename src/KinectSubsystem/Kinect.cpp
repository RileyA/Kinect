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
		,mDepthBuffer(0)
		,mColorBuffer(0)
		,mColorTime(0)
		,mDepthTime(0)
	{
		// zero out the tilt
		setTiltAngle(0);

		// set callbacks
		freenect_set_video_callback(mDevice, Oryx::colorCallback);
		freenect_set_depth_callback(mDevice, Oryx::depthCallback);

		// choose video modes
		freenect_set_video_mode(mDevice, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, 
			FREENECT_VIDEO_RGB));
		freenect_set_depth_mode(mDevice, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, 
			FREENECT_DEPTH_11BIT));
	}
	//-----------------------------------------------------------------------

	Kinect::~Kinect()
	{
		setVideoEnabled(false);
		setRGBDepthEnabled(false);
		freenect_close_device(mDevice);
	}
	//-----------------------------------------------------------------------
	
	void Kinect::setVideoEnabled(bool enabled)
	{
		setDepthOutputEnabled(enabled);
		setColorOutputEnabled(enabled);
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

	void Kinect::setRGBDepthEnabled(bool enabled)
	{
		if(mDepthBuffer && !mDepthRGBBuffer && enabled)
		{
			mDepthRGBBuffer = new byte[648*480*3];
		}
		else if(mDepthRGBBuffer && !enabled)
		{
			delete[] mDepthRGBBuffer;
			mDepthRGBBuffer = 0;
		}
	}
	//-----------------------------------------------------------------------

	void Kinect::setColorOutputEnabled(bool enabled)
	{
		if(!mColorBuffer && enabled)
		{
			mColorBuffer = new byte[648*480*3];
			freenect_set_video_buffer(mDevice, mColorBuffer);
			freenect_start_video(mDevice);
		}
		else if(mColorBuffer && !enabled)
		{
			freenect_stop_video(mDevice);
			freenect_set_video_buffer(mDevice, 0);
			delete[] mColorBuffer;
			mColorBuffer = 0;
		}
	}
	//-----------------------------------------------------------------------

	void Kinect::setDepthOutputEnabled(bool enabled)
	{
		if(!mDepthBuffer && enabled)
		{
			mDepthBuffer = new k_depth[648*480];
			freenect_set_depth_buffer(mDevice, mDepthBuffer);
			freenect_start_depth(mDevice);
		}
		else if(mDepthBuffer && !enabled)
		{
			freenect_stop_depth(mDevice);
			freenect_set_depth_buffer(mDevice, 0);
			delete[] mDepthBuffer;
			mDepthBuffer = 0;
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
		return mDepthBuffer;
	}
	//-----------------------------------------------------------------------
	
	byte* Kinect::getRawColor()
	{
		return mColorBuffer;
	}
	//-----------------------------------------------------------------------
	
	uint32_t Kinect::getColorTimestamp()
	{
		return mColorTime;
	}
	//-----------------------------------------------------------------------
	
	uint32_t Kinect::getDepthTimestamp()
	{
		return mDepthTime;
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
	
	void Kinect::depthCallback(freenect_device* device, void *data, uint32_t timestamp)
	{
		mDepthTime = timestamp;

		// if we're outputting to the depth rgb, then process the raw depth into a
		// color stream
		if(mDepthRGBBuffer)
		{
			k_depth* depth = static_cast<k_depth*>(data);
			
			for(int i = 0; i< 640*480; ++i)
			{
				// simply constrain to 0-255 for now...
				byte intensity = static_cast<byte>((mDepths[depth[i]]/5.f)*255.f);

				mDepthRGBBuffer[i*3+0] = intensity;
				mDepthRGBBuffer[i*3+1] = intensity;
				mDepthRGBBuffer[i*3+2] = intensity;
			}
		}
	}
	//-----------------------------------------------------------------------

	void Kinect::colorCallback(freenect_device* device, void *data, uint32_t timestamp)
	{
		mColorTime = timestamp;
	}
	//-----------------------------------------------------------------------
	
	void depthCallback(freenect_device *dev, void *v_depth, uint32_t timestamp)
	{
		// get the device the callback originated from and call the callback on that device
		Engine::getPtr()->getSubsystem("KinectSubsystem")->
			castType<KinectSubsystem>()->getDevice(dev)->depthCallback(dev, v_depth, timestamp);
	}
	//-----------------------------------------------------------------------

	void colorCallback(freenect_device *dev, void *rgb, uint32_t timestamp)
	{
		// get the device the callback originated from and call the callback on that device
		Engine::getPtr()->getSubsystem("KinectSubsystem")->
			castType<KinectSubsystem>()->getDevice(dev)->colorCallback(dev, rgb, timestamp);
	}
	//-----------------------------------------------------------------------
}
