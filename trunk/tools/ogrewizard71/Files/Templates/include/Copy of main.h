/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright © 2000-2004 The OGRE Team
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
#include "OgreStringConverter.h"

// basic event handler
[!if FRAMEWORK_OWN]
class [!output PROJECT_NAME]FrameListener : public BaseFrameListener
[!else]
class [!output PROJECT_NAME]FrameListener : public ExampleFrameListener
[!endif]
{
protected:
[!if FRAMELISTENER_YES]
	SceneManager *mSceneMgr;
[!endif]
public:
[!if FRAMELISTENER_YES]
[!if FRAMEWORK_OWN]
	[!output PROJECT_NAME]FrameListener(SceneManager *sceneMgr, RenderWindow* win, Camera* cam)
		: BaseFrameListener(win, cam)
[!else]
	[!output PROJECT_NAME]FrameListener(SceneManager *sceneMgr, RenderWindow* win, Camera* cam)
		: ExampleFrameListener(win, cam)
[!endif]
[!else]
[!if FRAMEWORK_OWN]
[!output PROJECT_NAME]FrameListener(RenderWindow* win, Camera* cam)
: BaseFrameListener(win, cam)
[!else]
[!output PROJECT_NAME]FrameListener(RenderWindow* win, Camera* cam)
: ExampleFrameListener(win, cam)
[!endif]
[!endif]
	{
	[!if FRAMELISTENER_YES]
		mSceneMgr = sceneMgr;
	[!endif]
	}

	bool frameStarted(const FrameEvent& evt)
	{
[!if FRAMELISTENER_YES]
		float moveScale;
		float rotScale;
		static bool wireframe = false;

		// local just to stop toggles flipping too fast
		static Real timeUntilNextToggle = 0;

		if (timeUntilNextToggle >= 0)
		timeUntilNextToggle -= evt.timeSinceLastFrame;

		// If this is the first frame, pick a speed
		if (evt.timeSinceLastFrame == 0)
		{
			moveScale = 1;
			rotScale = 0.1;
		} // if (evt.timeSinceLastFrame == 0)
		// Otherwise scale movement units by time passed since last frame
		else
		{
			// Move about 2 units per second,
			moveScale = 50 * evt.timeSinceLastFrame;
			// Take about 10 seconds for full rotation
			rotScale = 36 * evt.timeSinceLastFrame;
		}

		// Grab input device state
		mInputDevice->capture();

		static Vector3 vec;

		vec = Vector3::ZERO;

		if (mInputDevice->isKeyDown(KC_A))
		{
			// Move camera left
			vec.x = -moveScale;
		}

		if (mInputDevice->isKeyDown(KC_D))
		{
			// Move camera RIGHT
			vec.x = moveScale;
		}

		if (mInputDevice->isKeyDown(KC_UP) || mInputDevice->isKeyDown(KC_W))
		{
			// Move camera forward
			vec.z = -moveScale;
		}

		if (mInputDevice->isKeyDown(KC_DOWN) || mInputDevice->isKeyDown(KC_S))
		{
			// Move camera backward
			vec.z = moveScale;
		}

		if (mInputDevice->isKeyDown(KC_PGUP))
		{
			// Move camera up
			vec.y = moveScale;
		}

		if (mInputDevice->isKeyDown(KC_PGDOWN))
		{
			// Move camera down
			vec.y = -moveScale;
		}

		if (mInputDevice->isKeyDown(KC_RIGHT))
		{
			mCamera->yaw(-rotScale);
		}
		if (mInputDevice->isKeyDown(KC_LEFT))
		{
			mCamera->yaw(rotScale);
		}

		if( mInputDevice->isKeyDown( KC_ESCAPE) )
		{            
			return false;
		}

		// Rotate view by mouse relative position
		float rotX, rotY;
		rotX = -mInputDevice->getMouseRelativeX() * 0.13;
		rotY = -mInputDevice->getMouseRelativeY() * 0.13;


		// Make all the changes to the camera
		// Note that YAW direction is around a fixed axis (freelook stylee) rather than a natural YAW (e.g. airplane)
		mCamera->yaw(rotX);
		mCamera->pitch(rotY);
		mCamera->moveRelative(vec);

		if (mInputDevice->isKeyDown(KC_F) && timeUntilNextToggle <= 0)
		{
			mStatsOn = !mStatsOn;
			showDebugOverlay(mStatsOn);

			timeUntilNextToggle = 1;
		}

		if (mInputDevice->isKeyDown(KC_T) && timeUntilNextToggle <= 0)
		{
			wireframe = !wireframe;
			if (wireframe)
				mCamera->setDetailLevel(SDL_WIREFRAME);
			else
				mCamera->setDetailLevel(SDL_SOLID);

			timeUntilNextToggle = 0.5;
		}

		// Return true to continue rendering
		return true;
[!else]
	[!if FRAMEWORK_OWN]
		return BaseFrameListener::frameStarted(evt);
	[!else]
		return ExampleFrameListener::frameStarted(evt);
	[!endif]
[!endif]
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

[!if SKIP_CONFIG]
	virtual void go(void)
	{
		if (!setup())
			return;

		mRoot->startRendering();
	}
[!endif]
protected:

[!if SKIP_CONFIG]
	virtual bool setup(void)
	{
		mRoot = new Root();

		setupResources();

		bool carryOn = configure();
		if (!carryOn) return false;

		chooseSceneManager();
		createCamera();
		createViewports();

		// Set default mipmap level (NB some APIs ignore this)
		TextureManager::getSingleton().setDefaultNumMipMaps(5);

		// Create the scene
		createScene();

		createFrameListener();

		return true;

	}

	virtual bool configure(void)
	{
		// Show the configuration dialog and initialise the system
		// You can skip this and use root.restoreConfig() to load configuration
		// settings if you were sure there are valid ones saved in ogre.cfg
		if(mRoot->restoreConfig())
		{
			mWindow = mRoot->initialise(true);
			return true;
		}
		else
		{
			if(mRoot->showConfigDialog())
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
	}
[!endif]

	// Just override the mandatory create scene method
	void createScene(void)
	{
	[!if SCENE_YES]
		// Set ambient light
		mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));

		// Create a skydome
		mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

		// Create a light
		Light* l = mSceneMgr->createLight("MainLight");
		// Accept default settings: point light, white diffuse, just set position
		// NB I could attach the light to a SceneNode if I wanted it to move automatically with
		//  other objects, but I don't
		l->setPosition(20,80,50);

		Entity *ent;

		// Define a floor plane mesh
		Plane p;
		p.normal = Vector3::UNIT_Y;
		p.d = 200;
		MeshManager::getSingleton().createPlane("FloorPlane",p,200000,200000,20,20,true,1,50,50,Vector3::UNIT_Z);

		// Create an entity (the floor)
		ent = mSceneMgr->createEntity("floor", "FloorPlane");
		ent->setMaterialName("Examples/RustySteel");
		// Attach to child of root node, better for culling (otherwise bounds are the combination of the 2)
		mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

		// Add a head, give it it's own node
		SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		ent = mSceneMgr->createEntity("head", "ogrehead.mesh");
		headNode->attachObject(ent);

		// Create the camera node & attach camera
		SceneNode* camNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		camNode->attachObject(mCamera);
		// Put in a bit of fog for the hell of it
		mSceneMgr->setFog(FOG_EXP, ColourValue::White, 0.0002);
	[!endif]
	}

	// Create new frame listener
	void createFrameListener(void)
	{
	[!if FRAMELISTENER_YES]
		mFrameListener= new [!output PROJECT_NAME]FrameListener(mSceneMgr, mWindow, mCamera);
	[!else]
		mFrameListener= new [!output PROJECT_NAME]FrameListener(mWindow, mCamera);
	[!endif]
		mRoot->addFrameListener(mFrameListener);
	}

};

#endif // __[!output PROJECT_NAME]_h_