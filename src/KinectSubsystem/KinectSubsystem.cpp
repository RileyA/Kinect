#include "Oryx.h"
#include "KinectSubsystem.h"
#include "OryxLogger.h"
#include "OryxEngine.h"
#include "OryxTimeManager.h"
#include "libfreenect.h"

namespace Oryx
{
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
			Logger::getPtr()->logMessage("Kinect Subsystem Starting...");

			if(freenect_init(&mContext, 0) >= 0)
			{
				mInitialized = true;

				Kinect::preprocessDepth();

				// verbose-ish log-age
				freenect_set_log_level(mContext, FREENECT_LOG_DEBUG);

				// look for devices
				Logger::getPtr()->logMessage("Found "+StringUtils::toString(getNumDevices()) + 
					" devices.");
				
				Logger::getPtr()->logMessage("Kinect init succeeded!");
			}
			else
			{
				mInitialized = false;
				mContext = 0;
				Logger::getPtr()->logMessage("Kinect init failed!");
			}
		}
	}
	//-----------------------------------------------------------------------

	void KinectSubsystem::_deinit()
	{
		if(mInitialized)
		{
			Logger::getPtr()->logMessage("Shutting down Kinect Subsystem...");

			for(int i = 0; i < mDevices.size(); ++i)
				delete mDevices[i];
			mDevices.clear();

			if(mContext)
			{
				freenect_shutdown(mContext);
				mContext = 0;
			}
			
			mInitialized = false;
			Logger::getPtr()->logMessage("Kinect Subsystem Deinitialized.");
		}
	}
	//-----------------------------------------------------------------------

	void KinectSubsystem::_update(Real delta)
	{
		// no need to update if there aren't any devices active
		if(!mDevices.empty())
			freenect_process_events(mContext);
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

	unsigned int KinectSubsystem::getNumDevices()
	{
		return freenect_num_devices(mContext);
	}

	Kinect* KinectSubsystem::initDevice(unsigned int deviceIndex)
	{
		if(getNumDevices() <= deviceIndex)
		{
			Logger::getPtr()->logMessage("Invalid index. No such device.");
			return 0;
		}

		if(getDevice(deviceIndex))
		{
			Logger::getPtr()->logMessage("Device at index " + StringUtils::toString(deviceIndex) +
				" is already in use.");
			return 0;
		}

		freenect_device* dev = 0;

		if(freenect_open_device(mContext, &dev, deviceIndex) >= 0 && dev)
		{
			mDevices[deviceIndex] = new Kinect(dev, mContext);
			return mDevices[deviceIndex];
		}
		else
		{
			Logger::getPtr()->logMessage("Could not initialize Kinect at index " + 
				StringUtils::toString(deviceIndex));
		}

		return 0;
	}
	//-----------------------------------------------------------------------
	
	void KinectSubsystem::deinitDevice(Kinect* device)
	{
		std::map<unsigned int,Kinect*>::iterator it = mDevices.begin();
		for(it; it != mDevices.end(); ++it)
		{
			if(device == it->second)
			{
				delete it->second;
				mDevices.erase(it);
			}
		}
	}
	//-----------------------------------------------------------------------

	Kinect* KinectSubsystem::getDevice(unsigned int index)
	{
		return mDevices.find(index) != mDevices.end() ? mDevices[index] : 0;
	}
	//-----------------------------------------------------------------------
	
	Kinect* KinectSubsystem::getDevice(freenect_device* handle)
	{
		std::map<unsigned int,Kinect*>::iterator it = mDevices.begin();
		for(it; it != mDevices.end(); ++it)
		{
			if(handle == it->second->getHandle())
				return it->second;
		}	
		return 0;
	}
	//-----------------------------------------------------------------------

	/*void KinectSubsystem::depthCallback(freenect_device* device, void *data, uint32_t time)
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
			int pval = mGamma[depth[i]];
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
			}
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
	*/
}
