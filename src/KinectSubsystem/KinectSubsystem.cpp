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
		:mContext(0) {}
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
				freenect_set_log_level(mContext, FREENECT_LOG_DEBUG);

				int nrDevices = freenect_num_devices(mContext);
				Logger::getPtr()->logMessage("Found "+StringUtils::toString(nrDevices)+" devices.");

				if(nrDevices < 0 || freenect_open_device(mContext, &mDevice, 0) < 0)
				{
					mInitialized = false;
					Logger::getPtr()->logMessage("Kinect init failed!");
				}
				else
				{
					// clear these out to begin with
					memset(mDepthBuffer_0, (byte)0, sizeof(mDepthBuffer_0));
					memset(mDepthBuffer_1, (byte)0, sizeof(mDepthBuffer_1));
					memset(mColorBuffer_0, (byte)0, sizeof(mColorBuffer_0));
					memset(mColorBuffer_1, (byte)0, sizeof(mColorBuffer_1));

					// set up the device (this should be broken into a separate class/file at some point...
					freenect_set_tilt_degs(mDevice, 0);
					freenect_set_led(mDevice,LED_RED);
					freenect_set_depth_callback(mDevice, Oryx::depthCallback);
					freenect_set_video_callback(mDevice, Oryx::colorCallback);
					freenect_set_video_mode(mDevice, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, 
						FREENECT_VIDEO_RGB));
					freenect_set_depth_mode(mDevice, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, 
						FREENECT_DEPTH_11BIT));
					freenect_set_video_buffer(mDevice, mColorBuffer_0);
					freenect_set_depth_buffer(mDevice, mDepthBuffer_0);
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
			// dump depth data to the stream
            mInitialized = false;
            Logger::getPtr()->logMessage("Kinect Subsystem Deinitialized.");
        }
    }
    //-----------------------------------------------------------------------

    void KinectSubsystem::_update(Real delta)
    {
		// do stuff here....
		if(!mInitialized || freenect_process_events(mContext) < 0)
		{
			Logger::getPtr()->logMessage("Kinect stopped processing events...");
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
		
	}
	//-----------------------------------------------------------------------

	void KinectSubsystem::colorCallback(freenect_device* device, void *data, uint32_t time)
	{

	}
	//-----------------------------------------------------------------------

	byte* KinectSubsystem::getRawDepth()
	{
		return &mDepthBuffer_0[0][0][0];
	}
	//-----------------------------------------------------------------------
	
	byte* KinectSubsystem::getRawColor()
	{
		return &mColorBuffer_0[0][0][0];
	}
	//-----------------------------------------------------------------------
}
