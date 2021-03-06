#ifndef KINECT_H
#define KINECT_H

#include "Oryx.h"
#include "Oryx3DMath.h"
#include "OryxMatrix3.h"
#include "Kinectdllmain.h"
#include "OryxEngineSubsystem.h"

#define KINECT_FOV_H 58.f
#define KINECT_FOV_H_RAD 1.01229f
#define KINECT_FOV_V 45.f
#define KINECT_FOV_V_RAD 0.78539f

// pre-declare these here, w/o actually including anything, so client
// aps don't need to include any libfreenect junk
typedef struct _freenect_device freenect_device;
typedef struct _freenect_context freenect_context;

namespace Oryx
{
	// For added clarity. Note that depth values are actually only 11-bits (0-2047)
	typedef uint16_t k_depth; 

	/** Represents a Microsoft Kinect device, wraps over the libfreenect C API */
	class Kinect
	{
	public:

		/** Represents the range of colors the Kinect's LED may be set to
		 *		Supposedly more colors can be obtained by rapidly switching 
		 *		between these defaults, so a later development may be to allow
		 *		automatic handling of such switching... */
		enum LEDColor
		{
			LC_OFF,
			LC_GREEN,
			LC_RED,
			LC_YELLOW,
			LC_BLINK_YELLOW,
			LC_BLINK_RED_YELLOW=6
		};

		/** Destructor */
		virtual ~Kinect();

		/** Shortcut to enable both depth and color streams */
		void setVideoEnabled(bool enabled);

		/** Sets the Kinect's LED color 
				@param color The desired color */
		void setLED(LEDColor color);

		/** Gets the current LED color */
		LEDColor getLED();

		/** Sets the Kinect's tilting value in degrees */
		void setTiltAngle(Real degrees);

		/** Gets the Kinect's tilting value in degrees */
		Real getTiltAngle();

		/** Enables an rgb color buffer based on the raw depth data
		 *		@remarks Require a good deal of extra memory and processing, 
		 *			so use with care.*/
		void setRGBDepthEnabled(bool enabled);

		/** Sets whether or not depth output is enabled, and available via
		 *		getRawDepth() */
		void setDepthOutputEnabled(bool enabled);

		/** Sets whether or not color output is enabled and available via
		 *		getRawColor */
		void setColorOutputEnabled(bool enabled);

		/** Gets the color depth output (or NULL if it is disabled)
		 *		@return Ptr to first element of a 640*480*3 array of 8-bit color vals */
		byte* getRGBDepth();

		/** Gets a pointer to the raw depth stream
		 *		@return Pointer to the first element of a 640*480 array of 
		 *			16-bit unsigned ints, values will range 0-2047 (11-bit)  */
		k_depth* getRawDepth();

		/** Gets a pointer to the raw color stream
		 *		@return A pointer to the first element of a 640*480*3 array of
		 *			color values (plain 'ol 24-bit RGB) */
		byte* getRawColor();

		/** Gets a timestamp for the last color stream update, to avoid redundant updates */
		uint32_t getColorTimestamp();

		/** Gets a timestamp for the last depth stream update, to avoid redundant updates */
		uint32_t getDepthTimestamp();

		/** Gets the libfreenect handle for the device */
		freenect_device* getHandle(){return mDevice;}

		/** Helper that approximates depth in meters based on raw Kinect output */
		static inline Real getApproxDepth(k_depth d)
		{
			return mDepths[d];
		}

		static inline Vector3 getApproxPos(Real x, Real y, k_depth raw_d)
		{
			// depth in meters
			Real d = Kinect::getApproxDepth(raw_d);

			// the view space position
			Vector3 vsPos = Vector3(x,y,d);

			// inverse view projection (transforms view -> world space)
			Matrix3 invVProj = Matrix3(tan(KINECT_FOV_H_RAD/2)*d,0,0,
					0,-tan(KINECT_FOV_V_RAD/2)*d,0,
					0,0,-1);

			// multiply into world space and return
			return vsPos * invVProj;
		}

		static inline Vector3 getApproxPos(int index, k_depth raw_d)
		{
			return getApproxPos((index % 640 - 320) / 320.f,
				(index / 640 - 240) / 240.f, raw_d);
		}

		// Allow the subsystem access to this thing's innards
		friend class KinectSubsystem;

		void depthCallback(freenect_device* device, void *data, uint32_t timestamp);
		void colorCallback(freenect_device* device, void *data, uint32_t timestamp);

	protected:

		/** Constructor, not to be called except by the KinectSubsystem itself
		 *		@param device The libfrenect device handle
		 *		@param context The libfrenect context this device belong to */
		Kinect(freenect_device* device, freenect_context* context);

		/** Calculates raw kinect->approx depth conversions. */
		static void preprocessDepth();

		/** Whether or not the color and depth streams are active */
		bool mVideoEnabled;
		
		/** The device itself */
		freenect_device* mDevice;

		/** Context this thing belongs to */
		freenect_context* mContext;

		/** I'm unsure as to the default atm, but libfreenect doesn't provide
		 *		a means to get the current value, so we'll store it here for now... */
		LEDColor mCurrentLED;

		// The 24-bit color video feed
		byte* mColorBuffer;

		/** 11-bit raw output, not suitable for use as a texture
		 *		N.B. This is not in any specific unit, this is magical raw output... */
		k_depth* mDepthBuffer;

		/** An extra bufer foir storing a color representation of the depth,
		 *		for use in visualizations and debugging */
		byte* mDepthRGBBuffer;

		// Precalculated distances
		static Real mDepths[2048];

		// timestamps
		uint32_t mColorTime, mDepthTime;
	};

	void depthCallback(freenect_device *dev, void *v_depth, uint32_t timestamp);
	void colorCallback(freenect_device *dev, void *rgb, uint32_t timestamp);
}

#endif
