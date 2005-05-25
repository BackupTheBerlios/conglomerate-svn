/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2005 The OGRE Team
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/

/*
-----------------------------------------------------------------------------
Filename:    [!output PROJECT_NAME].h
Description: A place for me to try out stuff with OGRE.
-----------------------------------------------------------------------------
*/
#ifndef __[!output PROJECT_NAME]_h_
#define __[!output PROJECT_NAME]_h_

[!if FRAMEWORK_OWN]
#include "BaseApplication.h"
[!else]
#include "ExampleApplication.h"
[!endif]

RaySceneQuery* raySceneQuery = 0;

// Event handler to add ability to alter curvature
[!if FRAMEWORK_OWN]
class [!output PROJECT_NAME]FrameListener : public BaseFrameListener
[!else]
class [!output PROJECT_NAME]FrameListener : public ExampleFrameListener
[!endif]
{
public:
[!if FRAMEWORK_OWN]
	[!output PROJECT_NAME]FrameListener(SceneManager *sceneMgr, RenderWindow* win, Camera* cam)
		: BaseFrameListener(win, cam)
[!else]
	[!output PROJECT_NAME]FrameListener(SceneManager *sceneMgr, RenderWindow* win, Camera* cam)
		: ExampleFrameListener(win, cam)
[!endif]
	{
		// Reduce move speed
		mMoveSpeed = 50;

	}

	bool frameStarted(const FrameEvent& evt)
	{
		// clamp to terrain
		[!if FRAMEWORK_OWN]
		bool ret = BaseFrameListener::frameStarted(evt);
		[!else]
		bool ret = ExampleFrameListener::frameStarted(evt);
		[!endif]
		static Ray updateRay;
		updateRay.setOrigin(mCamera->getPosition());
		updateRay.setDirection(Vector3::NEGATIVE_UNIT_Y);
		raySceneQuery->setRay(updateRay);
		RaySceneQueryResult& qryResult = raySceneQuery->execute();
		RaySceneQueryResult::iterator i = qryResult.begin();
		if (i != qryResult.end() && i->worldFragment)
		{
			SceneQuery::WorldFragment* wf = i->worldFragment;
			mCamera->setPosition(mCamera->getPosition().x,
				i->worldFragment->singleIntersection.y + 10,
				mCamera->getPosition().z);
		}

		return ret;

	}

};



[!if FRAMEWORK_OWN]
class [!output PROJECT_NAME]App : public BaseApplication
	[!else]
	class [!output PROJECT_NAME]App : public ExampleApplication
[!endif]
	{
	public:
		[!output PROJECT_NAME]App() {}

	~[!output PROJECT_NAME]App()
	{
		delete raySceneQuery;
	}

protected:


	virtual void chooseSceneManager(void)
	{
		// Get the SceneManager, in this case a generic one
		mSceneMgr = mRoot->getSceneManager( ST_EXTERIOR_CLOSE );
	}

	virtual void createCamera(void)
	{
		// Create the camera
		mCamera = mSceneMgr->createCamera("PlayerCam");

		// Position it at 500 in Z direction
		mCamera->setPosition(Vector3(128,25,128));
		// Look back along -Z
		mCamera->lookAt(Vector3(0,0,-300));
		mCamera->setNearClipDistance( 1 );
		mCamera->setFarClipDistance( 1000 );

	}


[!if SKIP_CONFIG]
    virtual bool configure(void)
    {
        // Show the configuration dialog and initialise the system
        // You can skip this and use root.restoreConfig() to load configuration
        // settings if you were sure there are valid ones saved in ogre.cfg
		if(mRoot->restoreConfig() || mRoot->showConfigDialog())
        {
            // If returned true, user clicked OK so initialise
            // Here we choose to let the system create a default rendering window by passing 'true'
            mWindow = mRoot->initialise(true);
            return true;
        }
        else
        {
            return false;
        }
    }
[!endif]


	// Just override the mandatory create scene method
	virtual void createScene(void)
	{
		Plane waterPlane;

		// Set ambient light
		mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

		// Create a light
		Light* l = mSceneMgr->createLight("MainLight");
		// Accept default settings: point light, white diffuse, just set position
		// NB I could attach the light to a SceneNode if I wanted it to move automatically with
		//  other objects, but I don't
		l->setPosition(20,80,50);

		// Fog
		// NB it's VERY important to set this before calling setWorldGeometry
		// because the vertex program picked will be different
		ColourValue fadeColour(0.93, 0.86, 0.76);
		mSceneMgr->setFog( FOG_LINEAR, fadeColour, .001, 500, 1000);
		mWindow->getViewport(0)->setBackgroundColour(fadeColour);

		std::string terrain_cfg("terrain.cfg");
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
		terrain_cfg = mResourcePath + terrain_cfg;
#endif
		mSceneMgr -> setWorldGeometry( terrain_cfg );
		// Infinite far plane?
		if (mRoot->getRenderSystem()->getCapabilities()->hasCapability(RSC_INFINITE_FAR_PLANE))
		{
			mCamera->setFarClipDistance(0);
		}

		// Define the required skyplane
		Plane plane;
		// 5000 world units from the camera
		plane.d = 5000;
		// Above the camera, facing down
		plane.normal = -Vector3::UNIT_Y;

		// Set a nice viewpoint
		mCamera->setPosition(707,2500,528);
		mCamera->setOrientation(Quaternion(-0.3486, 0.0122, 0.9365, 0.0329));
		//mRoot -> showDebugOverlay( true );

		raySceneQuery = mSceneMgr->createRayQuery(
			Ray(mCamera->getPosition(), Vector3::NEGATIVE_UNIT_Y));


	}
	// Create new frame listener
	void createFrameListener(void)
	{
		mFrameListener= new [!output PROJECT_NAME]FrameListener(mSceneMgr, mWindow, mCamera);
		mRoot->addFrameListener(mFrameListener);
	}

};

#endif // #ifndef __[!output PROJECT_NAME]_h_