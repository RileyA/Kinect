#include "PlayState.h"
#include "Oryx3DMath.h"
#include "OryxMatrix4.h"

namespace Oryx
{
	PlayState::PlayState()
	{
		mTimer = 0.f;
		mAngle = 0.f;
		mTime2 = 0.f;
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
		if(mTimer > 1.f)
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
		if(mOIS->wasKeyPressed("KC_HOME"))
			mOIS->toggleMouseGrab();
		if(mOIS->wasKeyPressed("KC_ESCAPE"))
		{
			mKinect->stop();
			mEngine->endCurrentState();
		}
	}

	void addVert(MeshData& d, Vector3 pos)
	{
		d.vertices.push_back(pos.x);
		d.vertices.push_back(pos.y);
		d.vertices.push_back(pos.z);
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
		//d.addTexcoordSet();
	
		for(int i=0;i<640*480;++i)
		{
			if(data[i] > 2046)
				continue;

			// actual polygonal mesh (sloooow)
			/*int left = data[i];
			int right = data[i+1];
			int d_left = data[i+640];
			int d_right = data[i+641];

			if(left > 2046 || right > 2046 || d_left > 2046 || d_right > 2046)
				continue;

			Vector3 l_pos = Kinect::getApproxPos(i, left);
			Vector3 r_pos = Kinect::getApproxPos(i+1, right);
			Vector3 dl_pos = Kinect::getApproxPos(i+640, d_left);
			Vector3 dr_pos = Kinect::getApproxPos(i+641, d_right);

			if(l_pos.squaredDistance(r_pos) > 0.1f ||
				dl_pos.squaredDistance(dr_pos) > 0.1f ||
				dl_pos.squaredDistance(r_pos) > 0.1f ||
				dr_pos.squaredDistance(l_pos) > 0.1f)
				continue;

			// clockwise winding
			// tri 1
			addVert(d, l_pos);
			addVert(d, r_pos);
			addVert(d, dr_pos);
			// tri 2
			addVert(d, dr_pos);
			addVert(d, dl_pos); 
			addVert(d, l_pos);*/

			int x = i % 640;
			int y = i / 640;

			addVert(d,Kinect::getApproxPos(i,data[i]));
			// get left, right
			Vector3 pos1 = Kinect::getApproxPos(i-1,data[i-1]);// one to the left
			Vector3 pos2 = Kinect::getApproxPos(i+640,data[i+640]);// one down
			Vector3 normal = pos1.crossProduct(pos2);
			normal.y = abs(normal.y);
			std::cout<<normal.angleBetween(Vector3::UNIT_Y)<<"\n";

			if(!((x >= 639 || x <= 0) && 
				(!(y >= 479 || y <= 0))))
			{
				// try and see if this is a part of the ground plane...
				d.diffuse.push_back(color[i*3]/255.f);
				d.diffuse.push_back(color[i*3+1]/255.f);
				d.diffuse.push_back(color[i*3+2]/255.f);
				d.diffuse.push_back(1.f);
			}
			else
			{
				d.diffuse.push_back(1.f);
				d.diffuse.push_back(1.f);
				d.diffuse.push_back(1.f);
				d.diffuse.push_back(1.f);
			}
			
			//for(int j = 0; j < 6; ++j)
			//{
				//d.diffuse.push_back(color[i*3]/255.f);
				//d.diffuse.push_back(color[i*3+1]/255.f);
				//d.diffuse.push_back(color[i*3+2]/255.f);
				//d.diffuse.push_back(1.f);
			//}
		}

		if(!mMesh)
		{
			mMesh = mOgre->createMesh(d);
			mOgre->getRootSceneNode()->addChild(mMesh);
			mMesh->setMaterialName("cloud");
		}
		else
		{
			mMesh->update(d);
		}
	}
}
