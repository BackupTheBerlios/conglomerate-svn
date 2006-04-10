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
Filename:    OgreOpcodeTerrainExample.h
Description: A place for me to try out stuff with OGRE.
-----------------------------------------------------------------------------
*/
#ifndef __OgreOpcodeTerrainExample_h_
#define __OgreOpcodeTerrainExample_h_

// Include OgreOpcode headers.
#include "OgreOpcode.h"

using namespace OgreOpcode;
#include "ExampleApplication.h"

#include "OgreTerrainSceneManager.h"
#include "OgreTerrainPageSource.h"

class OgreOpcodeTerrainExampleFrameListener : public ExampleFrameListener, public TerrainPageSourceListener
{
private:
	SceneManager* mSceneMgr;
	CollisionContext* mCollideContext;
	TerrainCollisionShape* terrainShape;
	CollisionObject* terrainCollObj;
	bool mVisualizeObjects;
	bool mDoABBVisualization;
	bool mDoLocalVisualization;
	bool mDoGlobalVisualization;
	Overlay* mTargetSight;
	Overlay* mHotTargetSight;
	Overlay* mInfoOverlay;
	OverlayElement* mRayObjectText;
	OverlayElement* mRayDistanceText;
	Ray mRay;
	String mDbgMsg1;
	String mDbgMsg2;

public:
	OgreOpcodeTerrainExampleFrameListener(SceneManager *sceneMgr, RenderWindow* win, Camera* cam)
		: ExampleFrameListener(win, cam),
		mSceneMgr(sceneMgr),
		mCollideContext(0)
	{
		mTargetSight = (Overlay*)OverlayManager::getSingleton().getByName("gunTarget");
		mTargetSight->show();
		mHotTargetSight = (Overlay*)OverlayManager::getSingleton().getByName("hotGunTarget");
		mHotTargetSight->hide();


		mInfoOverlay = (Overlay*)OverlayManager::getSingleton().getByName("OgreOpcode/InfoOverlay");
		mInfoOverlay->show();

		mRayObjectText = OverlayManager::getSingleton().getOverlayElement("OgreOpcodeExample/Demo/RayIsHitting");
		mRayObjectText->setCaption("None");

		mRayDistanceText = OverlayManager::getSingleton().getOverlayElement("OgreOpcodeExample/Demo/RayDistance");
		mRayDistanceText->setCaption("None");

		// Move the debug overlay up a bit so we can get 2 lines in.
		OverlayElement* dbgTxt = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
		dbgTxt->setTop(dbgTxt->getTop()-dbgTxt->getHeight());

		new CollisionManager(mSceneMgr);
		CollisionManager::getSingletonPtr()->beginCollClasses();
		CollisionManager::getSingletonPtr()->addCollClass("level");
		CollisionManager::getSingletonPtr()->addCollClass("bullet");
		CollisionManager::getSingletonPtr()->addCollClass("ogrerobot");
		CollisionManager::getSingletonPtr()->addCollClass("powerup");
		CollisionManager::getSingletonPtr()->endCollClasses();

		CollisionManager::getSingletonPtr()->beginCollTypes();
		CollisionManager::getSingletonPtr()->addCollType("level", "level", COLLTYPE_IGNORE);
		CollisionManager::getSingletonPtr()->addCollType("ogrerobot", "bullet", COLLTYPE_EXACT);
		CollisionManager::getSingletonPtr()->addCollType("level", "ogrerobot", COLLTYPE_EXACT);
		CollisionManager::getSingletonPtr()->addCollType("level", "powerup", COLLTYPE_QUICK);
		CollisionManager::getSingletonPtr()->addCollType("level", "ogrerobot", COLLTYPE_EXACT);
		CollisionManager::getSingletonPtr()->addCollType("powerup", "powerup", COLLTYPE_IGNORE);
		CollisionManager::getSingletonPtr()->addCollType("powerup", "bullet", COLLTYPE_IGNORE);
		CollisionManager::getSingletonPtr()->addCollType("bullet", "bullet", COLLTYPE_IGNORE);
		CollisionManager::getSingletonPtr()->endCollTypes();

		mCollideContext = CollisionManager::getSingletonPtr()->getDefaultContext();

		terrainShape = CollisionManager::getSingletonPtr()->createTerrainCollisionShape("terrainShape");
		terrainCollObj = mCollideContext->newObject("terrainObject");
		terrainCollObj->setCollClass("level");
		terrainCollObj->setShape(terrainShape);
		mCollideContext->addObject(terrainCollObj);

		TerrainPageSource::addListener(this);

	}

	virtual void pageConstructed(TerrainSceneManager* pSceneMgr, size_t pagex, size_t pagez, Real* heightData) 
	{
		Vector3 Offset(0,0,0); // Set to something if required
		// Get parameters of the terrain
		Vector3 Scale = static_cast<TerrainSceneManager*>(pSceneMgr)->getScale();
		int iPageSize = static_cast<TerrainSceneManager*>(pSceneMgr)->getPageSize();
		
		// # of heightData should be iPageSize*iPageSize
		int heightDataNum = iPageSize * iPageSize;
		float* OOData;
		OOData = new float[heightDataNum*3];
		heightData = new Real[heightDataNum*3];
		int OODataCounter = 0;
		for(int x=0;x<iPageSize;++x)
		{
			for(int z=0;z<iPageSize;++z)
			{
				// set proper scale
				Vector3 Pt = Vector3(x,heightData[z*iPageSize+x],z)*Scale+Offset;
				// copy it over
				OOData[OODataCounter + 0] = Pt.x;
				OOData[OODataCounter + 1] = Pt.y;
				OOData[OODataCounter + 2] = Pt.z;
				OODataCounter += 3;
			}
		}
		// just checking ..
		assert(OODataCounter == heightDataNum*3);
		// pass OOData to OgreOpcode..
		terrainShape->load(heightDataNum, OOData);
	}

	bool frameStarted(const FrameEvent& evt)
	{
		static bool this_is_done = false;

		if(!this_is_done)
		{
			Real* theData = 0;
			pageConstructed(static_cast<TerrainSceneManager*>(mSceneMgr),0,0,theData);
			mCollideContext->reset();
			this_is_done = true;
		}

		bool ret = ExampleFrameListener::frameStarted(evt);

		mCollideContext->visualize(mVisualizeObjects, mDoABBVisualization, mDoLocalVisualization, mDoGlobalVisualization);

		mCollideContext->collide(evt.timeSinceLastFrame);

		mRay = mCamera->getCameraToViewportRay(0.5, 0.5);

		// remove level from context - we don't care when ray testing against entities..
		//CollisionManager::getSingletonPtr()->getDefaultContext()->removeObject(mCollObj1);

		// Do ray testing against everything but the level
		CollisionPair **pick_report;
		int num_picks = CollisionManager::getSingletonPtr()->getDefaultContext()->rayCheck(mRay, 2000.0f, COLLTYPE_CONTACT, COLLTYPE_ALWAYS_CONTACT, pick_report);

		mDbgMsg1 = "";
		mDbgMsg2 = "";
		if (num_picks > 0)
		{
			mTargetSight->hide();
			mHotTargetSight->show();
			mDbgMsg1 += Ogre::StringConverter::toString(num_picks) + " ";

			for(int i = 0; i < num_picks; i++)
			{
				CollisionObject* yeah = pick_report[0]->co_this;
				Vector3 contact = pick_report[0]->contact;
				mDbgMsg1 = mDbgMsg1 + yeah->getShape()->getName();
				mDbgMsg2 = mDbgMsg2 + " Distance: " + StringConverter::toString(pick_report[0]->distance);
			}
		}
		else
		{
			mTargetSight->show();
			mHotTargetSight->hide();
		}

		mDbgMsg2 = mDbgMsg2 + " \nCollisionContext attached objects: " + StringConverter::toString(mCollideContext->getAttachedObjectCount());
		mDbgMsg2 = mDbgMsg2 + " \nCollisionContext owned objects: " + StringConverter::toString(mCollideContext->getOwnedObjectCount());
		mDbgMsg2 = mDbgMsg2 + " \nCollisionManager shapes: " + StringConverter::toString(CollisionManager::getSingletonPtr()->getShapeCount());
		mRayObjectText->setCaption(mDbgMsg1);
		mRayDistanceText->setCaption(mDbgMsg2);

		return ret;

	}

	virtual bool processUnbufferedKeyInput(const FrameEvent& evt)
	{
		if (mInputDevice->isKeyDown(KC_V) && mTimeUntilNextToggle <=0)
		{
			mVisualizeObjects = !mVisualizeObjects;
			mTimeUntilNextToggle = 0.5;
		}

		if (mInputDevice->isKeyDown(KC_Z) && mTimeUntilNextToggle <= 0)
		{
			mDoABBVisualization = !mDoABBVisualization;
			mTimeUntilNextToggle = 0.5;
		}

		if (mInputDevice->isKeyDown(KC_X) && mTimeUntilNextToggle <= 0)
		{
			mDoLocalVisualization = !mDoLocalVisualization;
			mTimeUntilNextToggle = 0.5;
		}

		if (mInputDevice->isKeyDown(KC_C) && mTimeUntilNextToggle <= 0)
		{
			mDoGlobalVisualization = !mDoGlobalVisualization;
			mTimeUntilNextToggle = 0.5;
		}

		return ExampleFrameListener::processUnbufferedKeyInput(evt);
	}

};


class OgreOpcodeTerrainExampleApp : public ExampleApplication
{
public:
	OgreOpcodeTerrainExampleApp()
	{}

	~OgreOpcodeTerrainExampleApp()
	{
	}

protected:

	virtual void createCamera(void)
	{
		// Create the camera
		mCamera = mSceneMgr->createCamera("PlayerCam");

		// Position it at 500 in Z direction
		mCamera->setPosition(Vector3(0,0,80));
		// Look back along -Z
		mCamera->lookAt(Vector3(0,0,-300));
		mCamera->setNearClipDistance(5);
	}

	virtual void chooseSceneManager(void)
	{
		mSceneMgr = mRoot->createSceneManager("TerrainSceneManager");
	}

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


	// Just override the mandatory create scene method
	virtual void createScene(void)
	{

		Entity* ogreHead = mSceneMgr->createEntity("Head", "ogrehead.mesh");

		SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		headNode->attachObject(ogreHead);

		// Set ambient light
		mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

		// Create a light
		Light* l = mSceneMgr->createLight("MainLight");
		l->setPosition(20,80,50);

		mSceneMgr->setWorldGeometry("terrain.cfg");
	}

	// Create new frame listener
	void createFrameListener(void)
	{
		mFrameListener= new OgreOpcodeTerrainExampleFrameListener(mSceneMgr, mWindow, mCamera);
		mRoot->addFrameListener(mFrameListener);
	}
};

#endif // #ifndef __OgreOpcodeTerrainExample_h_