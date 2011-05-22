#include "kinect_demo.h"

#include <unistd.h>

class TestState : public GameState
{
public:

	TestState()
		:mColorTimestamp(0)
		,mDepthTimestamp(0)
	{
		//memset(drawn,0,sizeof(drawn));
		mKinect = mEngine->getSubsystem("KinectSubsystem")->castType<KinectSubsystem>();
		mSDL = mEngine->getSubsystem("SDLSubsystem")->castType<SDLSubsystem>();
	}

	~TestState()
	{

	}

	virtual void init()
	{
		mSDL->setManual(true);
		
		// init the device at index 0
		mDevice = mKinect->initDevice(0);

		// enable depth and color streams
		mDevice->setVideoEnabled(true);

		// enable the special depth rgb stream
		mDevice->setRGBDepthEnabled(true);

		mKinect->go();
	}

	virtual void deinit()
	{
		mKinect->deinitDevice(mDevice);
	}

	virtual void update(Real delta)
	{
		bool surfaceDirty = false;

		if(mColorTimestamp != mDevice->getColorTimestamp())
		{
			mColorTimestamp = mDevice->getColorTimestamp();
			mSDL->drawRaw(mDevice->getRawColor(),640,0);
			surfaceDirty = true;
		}
		if(mDepthTimestamp != mDevice->getDepthTimestamp())
		{
			mDepthTimestamp = mDevice->getDepthTimestamp();
			mSDL->drawRaw(mDevice->getRGBDepth(),0,0);
			surfaceDirty = true;

			// painting experiment:
			/*mSDL->lock();

			for(int i = 0; i<640*480; ++i)
			{
				if(depth[i]<750&& depth[i]>700 && depth[i]!=2047)
				{
					mSDL->drawPixel(640-i%640,i/640,255,0,0);
					drawn[i] = 1;
				}
				else if(drawn[i])
				{
					mSDL->drawPixel(640-i%640,i/640,0,100,0);
				}
				else if(depth[i]<875 && depth[i]!=2047)
				{
					mSDL->drawPixel(640-(i%640),i/640,50,0,0);
				}
				else
				{
					mSDL->drawPixel(640-(i%640),i/640,0,0,0);
				}
			}

			mSDL->unlock();*/
		}

		if(surfaceDirty)
			mSDL->flip();// flip the buffers

		// exit after 30secs, since I don't have any input set up...
		if(TimeManager::getPtr()->getTimeDecimal() > 10.f)
		{
			mEngine->endCurrentState();
			mKinect->stop();
		}
	}

private:

	KinectSubsystem* mKinect;
	SDLSubsystem* mSDL;
	Kinect* mDevice;

	uint32_t mColorTimestamp;
	uint32_t mDepthTimestamp;

	//byte drawn[640*480];

};

int main(int argc, char** argv)
{
	Logger::getPtr();
	TimeManager::getPtr();

	// create subsystems:
	KinectSubsystem kin = KinectSubsystem();
	SDLSubsystem sdl = SDLSubsystem(640*2, 480);

	// allocate engine and add subsystems
	Engine* eng = new Engine();
	eng->addSubsystem(&kin);
	eng->addSubsystem(&sdl);

	// initialize the engine
	eng->init();

	// add the test state to the deque
	eng->addState(new TestState);

	// start up the engine
	eng->start();

	// delete the engine object
	delete eng;

	return 0;
}
