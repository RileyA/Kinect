#include "kinect_demo.h"

#include <unistd.h>

class TestState : public GameState
{
public:

	TestState()
		:mColorTimestamp(0)
		,mDepthTimestamp(0)
	{
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
		//sleep(1);

		if(mColorTimestamp != mDevice->getColorTimestamp())
		{
			mColorTimestamp = mDevice->getColorTimestamp();
			mSDL->drawRaw(mDevice->getRawColor());
			surfaceDirty = true;
		}
		if(mDepthTimestamp != mDevice->getDepthTimestamp())
		{
			mDepthTimestamp = mDevice->getDepthTimestamp();
			mSDL->drawRaw(mDevice->getRGBDepth(), 640, 0);
			surfaceDirty = true;
			//mDevice->setLED(Kinect::LEDColor(Rand::get().gen(0,4)));
		}

		if(surfaceDirty)
			mSDL->flip();// flip the buffers

		// exit after 30secs, since I don't have any input set up...
		if(TimeManager::getPtr()->getTimeDecimal() > 30.f)
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
