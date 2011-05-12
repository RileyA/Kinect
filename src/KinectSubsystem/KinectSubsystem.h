#ifndef ORYX_KINECT_SUBSYSTEM_H
#define ORYX_KINECT_SUBSYSTEM_H

#include "Oryx.h"
#include "Kinectdllmain.h"
#include "OryxEngineSubsystem.h"
#include "Kinect.h"

namespace Oryx
{
	//void depthCallback(freenect_device *dev, void *v_depth, uint32_t timestamp);
	//void colorCallback(freenect_device *dev, void *rgb, uint32_t timestamp);

	class ORYX_KINECT_EXPORT KinectSubsystem : public EngineSubsystem
	{
	public:

		KinectSubsystem();
		~KinectSubsystem();

		void _init();
		void _deinit();
		void _update(Real delta);
		void _endState();

		String getName();

		/** Returns the number of Kinects libfreenect sees */
		unsigned int getNumDevices();

		/** Sets up a Kinect by index and returns a pointer (or NULL if not successful) */
		Kinect* initDevice(unsigned int deviceIndex);

		/** Shuts down a Kinect */
		void deinitDevice(Kinect* device);

		/** Gets an initialized device by index */
		Kinect* getDevice(unsigned int index);

		/** Gets a Kinect object pointer based on the libfreenect pointer
		 *		@remarks This is primarily for the depth/color callbacks. */
		Kinect* getDevice(freenect_device* handle);

		/*void depthCallback(freenect_device* device, void *data, uint32_t time);
		void colorCallback(freenect_device* device, void *data, uint32_t time);

		byte* getRawDepth();
		byte* getRawColor();*/
	
	protected:

		bool mInitialized;
		freenect_context* mContext;

		// The devices, a map is used here so that the device indices from libfreenect
		// align with the indices used to store these
		std::map<unsigned int, Kinect*> mDevices;

	};
}

#endif
