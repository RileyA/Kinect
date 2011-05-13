#include "kinect_demo.h"
#include "PlayState.h"

int main(int argc, char** argv)
{
	Logger::getPtr();
	TimeManager::getPtr();

	// create subsystems:
	KinectSubsystem kin = KinectSubsystem();
	OgreSubsystem ogre = OgreSubsystem(1024,768,false);
	OISSubsystem ois = OISSubsystem();
	
	// allocate engine and add subsystems
	Engine* eng = new Engine();
	eng->addSubsystem(&kin);
	eng->addSubsystem(&ogre);
	eng->addSubsystem(&ois);

	// initialize the engine
	eng->init();

	// add the test state to the deque
	eng->addState(new PlayState);

	// start up the engine
	eng->start();

	// delete the engine object
	delete eng;

	return 0;
}
