#include "PlayState.h"
#include "Oryx3DMath.h"

#define PI 3.14159f

// FOV, based on Kinect specs
#define KINECT_FOV_H 58.f
#define KINECT_FOV_H_RAD 58.f * PI/180.f
#define KINECT_FOV_V 45.f
#define KINECT_FOV_V_RAD 45.f * PI/180.f

// Complete guesses...
#define KINECT_NEAR 0.1
#define KINECT_FAR 0.7

namespace Oryx
{
	PlayState::PlayState()
	{
		mTimer = 0.f;
		mAngle = 0.f;
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

		mOIS->initInput(mOgre->getWindowHandle(), true);
		mDevice = mKinect->initDevice(0);
		mDevice->setVideoEnabled(true);
		mKinect->go();
		//mDevice->setRGBDepthEnabled(true);

		//mOgre->getRootSceneNode()->addChild(mOgre->createMesh("Frustum.mesh"));

		mOgre->setBackgroundColor(Colour(0.3f,0.3f,0.4f));

		mCamera = new FPSCamera();
	}

	void PlayState::deinit()
	{

	}

	void PlayState::update(Real delta)
	{
		mTimer+=delta;
		if(mTimer > 0.1f)
		{
			mTimer = 0.f;
			createPointCloud();
		}

		if(mOIS->wasKeyPressed("KC_1"))
			mDevice->setLED(Kinect::LC_RED);
		if(mOIS->wasKeyPressed("KC_2"))
			mDevice->setLED(Kinect::LC_GREEN);
		if(mOIS->wasKeyPressed("KC_3"))
			mDevice->setLED(Kinect::LC_OFF);

		if(mOIS->isKeyDown("KC_UP"))
		{
			mAngle += delta*5.f;
			mDevice->setTiltAngle(mAngle);
		}
		if(mOIS->isKeyDown("KC_DOWN"))
		{
			mAngle -= delta*5.f;
			mDevice->setTiltAngle(mAngle);
		}

		if(mOIS->wasKeyPressed("KC_P"))
			createPointCloud(false);
		if(mOIS->wasKeyPressed("KC_Q"))
			createPointCloud();
		if(mOIS->wasKeyPressed("KC_ESCAPE"))
		{
			mKinect->stop();
			mEngine->endCurrentState();
		}
	}

	void PlayState::createPointCloud(bool cap)
	{
		// process depth data and create mesh....
		k_depth* data;
		byte* color;

		if(cap)
		{
			data = mDevice->getRawDepth();
			color = mDevice->getRawColor();
			memcpy(storedColor, color, sizeof(640*480*3));
			memcpy(storedDepth, data, sizeof(640*480*2));
		}
		else
		{
			data = storedDepth;
			color = storedColor;
		}

		MeshData d = MeshData();
	
		for(int i=0;i<640*480;++i)
		{
			// if it's 2047, it means the device couldn't get a distance (shadow, IR interference, etc)
			Real dep = data[i] == 2047 ? 15 : Kinect::getApproxDepth(data[i]);

			if(data[i] > 2046)
				continue;

			// construct a view projection matrix, based on Kinect specs
			Matrix3 viewProjection = Matrix3::ZERO;

			Real w = 1 / tan(KINECT_FOV_H_RAD * 0.5f);
			Real h = 1 / tan(KINECT_FOV_V_RAD * 0.5f);
			Real q = KINECT_NEAR / (KINECT_FAR - KINECT_NEAR);

			viewProjection[0][0] = w;
			viewProjection[1][1] = h;
			viewProjection[2][2] = q;
			viewProjection[3][2] = -q * KINECT_NEAR;
			viewProjection[2][3] = 1;

			// view projection matrix
			Matrix3 invViewProj = viewProjection;//.Inverse();

			//Real de = 1/(-0.00307 * data[i] + 3.33);
			Vector3 viewSpacePos = Vector3((i%640-320)/640.f,-(i/640-240)/480.f,Kinect::getApproxDepth(data[i]));
			Vector3 worldSpacePos = /*invViewProj */ viewSpacePos;

			d.vertices.push_back(worldSpacePos.x * 2);
			d.vertices.push_back(worldSpacePos.y * 2);
			d.vertices.push_back(worldSpacePos.z * -1);
			
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
			//mMesh->yaw(180.f);
		}
		else
		{
			mMesh->update(d);
		}
	}
}
