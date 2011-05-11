#include "kinect_demo.h"

class TestState : public GameState
{
public:

	TestState()
	{
		mKinect = mEngine->getSubsystem("KinectSubsystem")->castType<KinectSubsystem>();
		mSDL = mEngine->getSubsystem("SDLSubsystem")->castType<SDLSubsystem>();
		mSDL->setManual(true);
	}

	~TestState()
	{

	}

	virtual void init()
	{
		// ...
	}

	virtual void deinit()
	{
		// ...
	}

	virtual void update(Real delta)
	{
		// draw kinect data to the SDL surface
		mSDL->drawRaw(mKinect->getRawDepth());
		mSDL->drawRaw(mKinect->getRawColor(),640,0);

		// exit after 30secs, since I don't have any input set up...
		if(TimeManager::getPtr()->getTimeDecimal() > 30.f)
			mEngine->endCurrentState();
	}

private:

	KinectSubsystem* mKinect;
	SDLSubsystem* mSDL;

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
