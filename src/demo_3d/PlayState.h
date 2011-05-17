#ifndef PLAY_STATE_H
#define PLAY_STATE_H

#include "kinect_demo.h"
#include "Oryx.h"
#include "OryxGameState.h"
#include "OryxEngine.h"
#include "FPSCamera.h"

namespace Oryx
{
	class PlayState : public GameState
	{
	public:

		PlayState();
		~PlayState();

		void init();
		void deinit();
		void update(Real delta);

		void createPointCloud(bool cap = true);

	private:

		KinectSubsystem* mKinect;
		OgreSubsystem* mOgre;
		OISSubsystem* mOIS;

		Kinect* mDevice;

		Mesh* mMesh;
		FPSCamera* mCamera;

		byte storedColor[640*480*3];
		k_depth storedDepth[640*480];

		Real mTimer;
		Real mAngle;
	};
}

#endif
