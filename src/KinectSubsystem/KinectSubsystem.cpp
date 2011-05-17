#include "Oryx.h"
#include "KinectSubsystem.h"
#include "OryxLogger.h"
#include "OryxEngine.h"
#include "OryxTimeManager.h"
#include "libfreenect.h"
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>

namespace Oryx
{
	KinectSubsystem::KinectSubsystem()
		:mContext(0),mDone(0) {}
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
			stop();
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
		//if(!mDevices.empty())
		//	freenect_process_events(mContext);
	}
	//-----------------------------------------------------------------------

	void KinectSubsystem::_endState()
	{

	}
	//-----------------------------------------------------------------------

	void KinectSubsystem::go()
	{
		boost::thread thrd(boost::bind(&KinectSubsystem::loop,this));
	}
	//-----------------------------------------------------------------------
	
	void KinectSubsystem::stop()
	{
		mDone = true;
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
	
	void KinectSubsystem::loop()
	{
		while(freenect_process_events(mContext) >= 0 && !mDone)
		{
			
		}
	}
	//-----------------------------------------------------------------------
}
