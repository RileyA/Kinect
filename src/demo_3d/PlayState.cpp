#include "PlayState.h"

namespace Oryx
{
	PlayState::PlayState()
	{
	}

	PlayState::~PlayState()
	{

	}

	void PlayState::init()
	{
		mMesh = 0;
		mKinect = mEngine->getSubsystem("KinectSubsystem")->castType<KinectSubsystem>();
		mOgre = mEngine->getSubsystem("OgreSubsystem")->castType<OgreSubsystem>();
		mOIS = mEngine->getSubsystem("OISSubsystem")->castType<OISSubsystem>();

		mOIS->initInput(mOgre->getWindowHandle(), false);
		mDevice = mKinect->initDevice(0);
		mDevice->setVideoEnabled(true);

		mCamera = new FPSCamera();
	}

	void PlayState::deinit()
	{

	}

	void PlayState::update(Real delta)
	{
		if(mOIS->wasKeyPressed("KC_Q"))
			createPointCloud();
		if(mOIS->wasKeyPressed("KC_ESCAPE"))
			mEngine->endCurrentState();
	}

	void PlayState::createPointCloud()
	{
		// process depth data and create mesh....
		k_depth* data = mDevice->getRawDepth();
		byte* color = mDevice->getRawColor();
		MeshData d = MeshData();
	
		for(int i=0;i<640*480;++i)
		{
			// 2047 menas it couldn't get a depth value (reflection, shadow, etc)
			if(data[i] == 2047)
				continue;

			d.vertices.push_back((i%640)/64.f);
			d.vertices.push_back((i/640)/64.f);
			d.vertices.push_back(Kinect::getApproxDepth(data[i])*3);
			
			d.diffuse.push_back(color[i*3]/255.f);
			d.diffuse.push_back(color[i*3+1]/255.f);
			d.diffuse.push_back(color[i*3+2]/255.f);
			d.diffuse.push_back(1.f);
		}

		if(!mMesh)
		{
			d.indices.clear();
			mMesh = mOgre->createMesh(d);
			mOgre->getRootSceneNode()->addChild(mMesh);
			mMesh->setMaterialName("cloud");
			mMesh->roll(180.f);
		}
		else
		{
			mMesh->update(d);
		}
	}
}
