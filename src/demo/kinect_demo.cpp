#include "kinect_demo.h"

class TestState : public GameState
{
public:

	TestState()
	{
		mKinect = mEngine->getSubsystem("KinectSubsystem")->castType<KinectSubsystem>();
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
		if(TimeManager::getPtr()->getTimeDecimal() > 3.f)
			mEngine->endCurrentState();
	}

private:

	KinectSubsystem* mKinect;

};

int main(int argc, char** argv)
{
	Logger::getPtr();
	TimeManager::getPtr();

	// create subsystems:
	KinectSubsystem kin = KinectSubsystem();

	// allocate engine and add subsystems
	Engine* eng = new Engine();
	eng->addSubsystem(&kin);

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
