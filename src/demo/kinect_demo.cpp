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
		// do stuff
	}

	virtual void deinit()
	{
		// ...
	}

	virtual void update(Real delta)
	{
		// ...

		mSDL->drawRaw(mKinect->getRawDepth());
		mSDL->drawRaw(mKinect->getRawColor(),640,0);

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

	// add the menu state to the stack
	eng->addState(new TestState);

	// start up the engine
	eng->start();

	// delete the engine object
	delete eng;

	return 0;
}
