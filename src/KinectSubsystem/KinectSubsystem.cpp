#include "Oryx.h"
#include "KinectSubsystem.h"
#include "OryxLogger.h"
#include "OryxEngine.h"
#include "OryxTimeManager.h"
#include "libfreenect.h"

namespace Oryx
{
	void depthCallback(freenect_device *dev, void *v_depth, uint32_t timestamp)
	{
		Engine::getPtr()->getSubsystem("KinectSubsystem")->
			castType<KinectSubsystem>()->depthCallback(dev, v_depth, timestamp);
	}

	void colorCallback(freenect_device *dev, void *rgb, uint32_t timestamp)
	{
		Engine::getPtr()->getSubsystem("KinectSubsystem")->
			castType<KinectSubsystem>()->colorCallback(dev, rgb, timestamp);
	}

	KinectSubsystem::KinectSubsystem()
		:mContext(0) 
	{
		// magical copy pasta from libfreenect's gltest
		// used for turning the raw depth data into nice colorful output
		//for (int i=0; i<2048; i++) 
		//{
		//	float v = i/2048.0;
		//	v = powf(v, 3)* 6;
		//	mGamma[i] = v*6*256;
		//}
	}
	//-----------------------------------------------------------------------

	KinectSubsystem::~KinectSubsystem()
    {
    }
    //-----------------------------------------------------------------------

    void KinectSubsystem::_init()
    {
        if(!mInitialized)
        {
            mInitialized = true;
            Logger::getPtr()->logMessage("Kinect Subsystem Starting...");

			if(freenect_init(&mContext, 0) >= 0)
			{
				// more verbose log-age
				freenect_set_log_level(mContext, FREENECT_LOG_DEBUG);

				// look for devices
				int nrDevices = freenect_num_devices(mContext);
				Logger::getPtr()->logMessage("Found "+StringUtils::toString(nrDevices)+" devices.");

				// TODO: do device stuff in a separate class

				// If there aren't any devices, or the first device fails to open, then abort
				if(nrDevices < 0 || freenect_open_device(mContext, &mDevice, 0) < 0)
				{
					mInitialized = false;
					Logger::getPtr()->logMessage("Kinect init failed!");
				}
				else
				{
					// clear these out to begin with
					memset(mDepthBuffer, (byte)0, sizeof(mDepthBuffer));
					memset(mColorBuffer, (byte)0, sizeof(mColorBuffer));

					// set the tilt (dunno how exactly this will work on the real thing...)
					freenect_set_tilt_degs(mDevice, 0);

					// ooooh, it has an LED!
					// freenect_set_led(mDevice,LED_RED);

					// register callbacks
					freenect_set_depth_callback(mDevice, Oryx::depthCallback);
					freenect_set_video_callback(mDevice, Oryx::colorCallback);
					
					// setup depth/color modes
					freenect_set_video_mode(mDevice, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, 
						FREENECT_VIDEO_RGB));
					freenect_set_depth_mode(mDevice, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, 
						FREENECT_DEPTH_11BIT));

					// set the buffer to use for video
					freenect_set_video_buffer(mDevice, mColorBuffer);

					// start 'em up
					freenect_start_depth(mDevice);
					freenect_start_video(mDevice);
				}
			}
			else
			{
				mInitialized = false;
				Logger::getPtr()->logMessage("Kinect init failed!");
			}

            if(mInitialized)
				Logger::getPtr()->logMessage("Kinect Subsystem Initialized.");
        }
    }
    //-----------------------------------------------------------------------

    void KinectSubsystem::_deinit()
    {
        if(mInitialized)
        {
            Logger::getPtr()->logMessage("Shutting down Kinect Subsystem...");

			// random cleanup
			freenect_stop_video(mDevice);
			freenect_stop_depth(mDevice);
			freenect_close_device(mDevice);
			freenect_shutdown(mContext);

            mInitialized = false;
            Logger::getPtr()->logMessage("Kinect Subsystem Deinitialized.");
        }
    }
    //-----------------------------------------------------------------------

    void KinectSubsystem::_update(Real delta)
    {
		if(!mInitialized || freenect_process_events(mContext) < 0)
		{
			// just exit if it stops processing events, or failed to init properly
			Engine::getPtr()->endCurrentState();
		}
    }
    //-----------------------------------------------------------------------

    void KinectSubsystem::_endState()
    {

    }
    //-----------------------------------------------------------------------

    String KinectSubsystem::getName()
    {
        return String("KinectSubsystem");
    }
    //-----------------------------------------------------------------------

	void KinectSubsystem::depthCallback(freenect_device* device, void *data, uint32_t time)
	{
		// depth comes as an 11 bit value (I think?)
		uint16_t *depth = (uint16_t*)data;

		byte* depthbuf = &mDepthBuffer[0][0][0];

		for(int i=0; i<640*480; i++) 
		{
			// scale into 0-255
			//byte dep = static_cast<byte>(depth[i]/2047.f * 255.f);

			// distance approximation in meters
			float distance = 0.1236 * tan(depth[i] / 2842.5 + 1.1863);
			byte dep = (std::min(distance, 5.f) / 5.f) * 255;

			// smear over all the channels...
			depthbuf[3*i+0] = dep;
			depthbuf[3*i+1] = dep;
			depthbuf[3*i+2] = dep;

			// the following was nabbed from the gltest example from freenect, and
			// formats the 11-bit depth data in a nice colorful format
			/*int pval = mGamma[depth[i]];
			int lb = pval & 0xff;
			
			switch (pval>>8) 
			{
				case 0:
					depthbuf[3*i+0] = 255;
					depthbuf[3*i+1] = 255-lb;
					depthbuf[3*i+2] = 255-lb;
					break;
				case 1:
					depthbuf[3*i+0] = 255;
					depthbuf[3*i+1] = lb;
					depthbuf[3*i+2] = 0;
					break;
				case 2:
					depthbuf[3*i+0] = 255-lb;
					depthbuf[3*i+1] = 255;
					depthbuf[3*i+2] = 0;
					break;
				case 3:
					depthbuf[3*i+0] = 0;
					depthbuf[3*i+1] = 255;
					depthbuf[3*i+2] = lb;
					break;
				case 4:
					depthbuf[3*i+0] = 0;
					depthbuf[3*i+1] = 255-lb;
					depthbuf[3*i+2] = 255;
					break;
				case 5:
					depthbuf[3*i+0] = 0;
					depthbuf[3*i+1] = 0;
					depthbuf[3*i+2] = 255-lb;
					break;
				default:
					depthbuf[3*i+0] = 0;
					depthbuf[3*i+1] = 0;
					depthbuf[3*i+2] = 0;
					break;
			}*/
		}
	}
	//-----------------------------------------------------------------------

	void KinectSubsystem::colorCallback(freenect_device* device, void *data, uint32_t time)
	{

	}
	//-----------------------------------------------------------------------

	byte* KinectSubsystem::getRawDepth()
	{
		return &mDepthBuffer[0][0][0];
	}
	//-----------------------------------------------------------------------
	
	byte* KinectSubsystem::getRawColor()
	{
		return &mColorBuffer[0][0][0];
	}
	//-----------------------------------------------------------------------
}
