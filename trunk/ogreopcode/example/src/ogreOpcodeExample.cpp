#include "ogreOpcodeExample.h"

#include "LoadingBar.h"

template<> OgreOpcodeExample* Ogre::Singleton<OgreOpcodeExample>::ms_Singleton = 0;

//-------------------------------------------------------------------------------------
OgreOpcodeExample::OgreOpcodeExample(void)
{
	mRoot = 0;
}

//-------------------------------------------------------------------------------------
OgreOpcodeExample::~OgreOpcodeExample(void)
{
	if (mInputTypeSwitchingOn)
	{
		delete mEventProcessor;
	}
	else
	{
		PlatformManager::getSingleton().destroyInputReader( mInputDevice );
	}
	if (mRoot)
		delete mRoot;
}

//-------------------------------------------------------------------------------------
bool OgreOpcodeExample::configure(void)
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
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::chooseSceneManager(void)
{
	// Get the SceneManager, in this case a generic one
	mSceneMgr = mRoot->getSceneManager(ST_GENERIC);
}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::createCamera(void)
{
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	mCamera->setPosition(Vector3(0,0,500));
	// Look back along -Z
	mCamera->lookAt(Vector3(0,0,-300));
	mCamera->setNearClipDistance(5);

}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::createFrameListener(void)
{
	mDebugOverlay = OverlayManager::getSingleton().getByName("OgreOpcodeExample/DebugOverlay");
	mUseBufferedInputKeys = false;
	mUseBufferedInputMouse = false;
	mInputTypeSwitchingOn = mUseBufferedInputKeys || mUseBufferedInputMouse;
	mRotateSpeed = 36;
	mMoveSpeed = 100;

	if (mInputTypeSwitchingOn)
	{
		mEventProcessor = new EventProcessor();
		mEventProcessor->initialise(mWindow);
		mEventProcessor->startProcessingEvents();
		mEventProcessor->addKeyListener(this);
		mInputDevice = mEventProcessor->getInputReader();

	}
	else
	{
		mInputDevice = PlatformManager::getSingleton().createInputReader();
		mInputDevice->initialise(mWindow,true, true);
	}

	mStatsOn = true;
	mNumScreenShots = 0;
	mTimeUntilNextToggle = 0;
	mSceneDetailIndex = 0;
	mMoveScale = 0.0f;
	mRotScale = 0.0f;
	mTranslateVector = Vector3::ZERO;
	mAniso = 1;
	mFiltering = TFO_BILINEAR;

	showDebugOverlay(true);
	mRoot->addFrameListener(this);
}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::createScene(void)
{
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
	MeshManager::getSingleton().createPlane(
		"FloorPlane", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, 
		p, 200000, 200000, 20, 20, true, 1, 50, 50, Vector3::UNIT_Z);

	// Create an entity (the floor)
	ent = mSceneMgr->createEntity("floor", "FloorPlane");
	ent->setMaterialName("Examples/RustySteel");
	// Attach to child of root node, better for culling (otherwise bounds are the combination of the 2)
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);

	// Add a head, give it it's own node
	SceneNode* headNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	ent = mSceneMgr->createEntity("head", "ogrehead.mesh");
	headNode->attachObject(ent);

	// Put in a bit of fog for the hell of it
	mSceneMgr->setFog(FOG_EXP, ColourValue::White, 0.0002);
}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::destroyScene(void)
{
}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::createViewports(void)
{
	// Create one viewport, entire window
	Viewport* vp = mWindow->addViewport(mCamera);
	vp->setBackgroundColour(ColourValue(0,0,0));

	// Alter the camera aspect ratio to match the viewport
	mCamera->setAspectRatio(
		Real(vp->getActualWidth()) / Real(vp->getActualHeight()));
}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::setupResources(void)
{
	// Load resource paths from config file
	ConfigFile cf;
	cf.load("resources.cfg");

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation(
				archName, typeName, secName);
		}
	}
}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::createResourceListener(void)
{

}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::loadResources(void)
{
	mLoadingBar = new LoadingBar();

	mLoadingBar->start(mWindow, 1, 1, 0.75);

	// Turn off rendering of everything except overlays
	mSceneMgr->clearSpecialCaseRenderQueues();
	mSceneMgr->addSpecialCaseRenderQueue(RENDER_QUEUE_OVERLAY);
	mSceneMgr->setSpecialCaseRenderQueueMode(SceneManager::SCRQM_INCLUDE);

	// Initialise the rest of the resource groups, parse scripts etc
	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	ResourceGroupManager::getSingleton().loadResourceGroup(
		ResourceGroupManager::getSingleton().getWorldResourceGroupName(),
		false, true);

	// Back to full rendering
	mSceneMgr->clearSpecialCaseRenderQueues();
	mSceneMgr->setSpecialCaseRenderQueueMode(SceneManager::SCRQM_EXCLUDE);

	mLoadingBar->finish();
	delete mLoadingBar;
	// Initialise, parse scripts etc
	//		ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::go(void)
{
	if (!setup())
		return;

	showDebugOverlay(true);

	mRoot->startRendering();

	// clean up
	destroyScene();
}
//-------------------------------------------------------------------------------------
bool OgreOpcodeExample::setup(void)
{
	mRoot = new Root();

	setupResources();

	bool carryOn = configure();
	if (!carryOn) return false;

	chooseSceneManager();
	createCamera();
	createViewports();

	// Set default mipmap level (NB some APIs ignore this)
	TextureManager::getSingleton().setDefaultNumMipmaps(5);

	// Create any resource listeners (for loading screens)
	createResourceListener();
	// Load resources
	loadResources();

	// Create the scene
	createScene();

	createFrameListener();

	return true;
};
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::updateStats(void)
{
	static String currFps = "Current FPS: ";
	static String avgFps = "Average FPS: ";
	static String bestFps = "Best FPS: ";
	static String worstFps = "Worst FPS: ";
	static String tris = "Triangle Count: ";

	// update stats when necessary
	try {
		OverlayElement* guiAvg = OverlayManager::getSingleton().getOverlayElement("Core/AverageFps");
		OverlayElement* guiCurr = OverlayManager::getSingleton().getOverlayElement("Core/CurrFps");
		OverlayElement* guiBest = OverlayManager::getSingleton().getOverlayElement("Core/BestFps");
		OverlayElement* guiWorst = OverlayManager::getSingleton().getOverlayElement("Core/WorstFps");

		const RenderTarget::FrameStats& stats = mWindow->getStatistics();

		guiAvg->setCaption(avgFps + StringConverter::toString(stats.avgFPS));
		guiCurr->setCaption(currFps + StringConverter::toString(stats.lastFPS));
		guiBest->setCaption(bestFps + StringConverter::toString(stats.bestFPS)
			+" "+StringConverter::toString(stats.bestFrameTime)+" ms");
		guiWorst->setCaption(worstFps + StringConverter::toString(stats.worstFPS)
			+" "+StringConverter::toString(stats.worstFrameTime)+" ms");

		OverlayElement* guiTris = OverlayManager::getSingleton().getOverlayElement("Core/NumTris");
		guiTris->setCaption(tris + StringConverter::toString(stats.triangleCount));

		OverlayElement* guiDbg = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
		guiDbg->setCaption(mWindow->getDebugText());
	}
	catch(...)
	{
		// ignore
	}
}
//-------------------------------------------------------------------------------------
bool OgreOpcodeExample::processUnbufferedKeyInput(const FrameEvent& evt)
{
	if (mInputDevice->isKeyDown(KC_A))
	{
		// Move camera left
		mTranslateVector.x = -mMoveScale;
	}

	if (mInputDevice->isKeyDown(KC_D))
	{
		// Move camera RIGHT
		mTranslateVector.x = mMoveScale;
	}

	/* Move camera forward by keypress. */
	if (mInputDevice->isKeyDown(KC_UP) || mInputDevice->isKeyDown(KC_W) )
	{
		mTranslateVector.z = -mMoveScale;
	}

	/* Move camera backward by keypress. */
	if (mInputDevice->isKeyDown(KC_DOWN) || mInputDevice->isKeyDown(KC_S) )
	{
		mTranslateVector.z = mMoveScale;
	}

	if (mInputDevice->isKeyDown(KC_PGUP))
	{
		// Move camera up
		mTranslateVector.y = mMoveScale;
	}

	if (mInputDevice->isKeyDown(KC_PGDOWN))
	{
		// Move camera down
		mTranslateVector.y = -mMoveScale;
	}

	if (mInputDevice->isKeyDown(KC_RIGHT))
	{
		mCamera->yaw(-mRotScale);
	}

	if (mInputDevice->isKeyDown(KC_LEFT))
	{
		mCamera->yaw(mRotScale);
	}

	if( mInputDevice->isKeyDown( KC_ESCAPE) )
	{
		return false;
	}

	// see if switching is on, and you want to toggle
	if (mInputTypeSwitchingOn && mInputDevice->isKeyDown(KC_M) && mTimeUntilNextToggle <= 0)
	{
		switchMouseMode();
		mTimeUntilNextToggle = 1;
	}

	if (mInputTypeSwitchingOn && mInputDevice->isKeyDown(KC_K) && mTimeUntilNextToggle <= 0)
	{
		// must be going from immediate keyboard to buffered keyboard
		switchKeyMode();
		mTimeUntilNextToggle = 1;
	}
	if (mInputDevice->isKeyDown(KC_F) && mTimeUntilNextToggle <= 0)
	{
		mStatsOn = !mStatsOn;
		showDebugOverlay(mStatsOn);

		mTimeUntilNextToggle = 1;
	}
	if (mInputDevice->isKeyDown(KC_T) && mTimeUntilNextToggle <= 0)
	{
		switch(mFiltering)
		{
		case TFO_BILINEAR:
			mFiltering = TFO_TRILINEAR;
			mAniso = 1;
			break;
		case TFO_TRILINEAR:
			mFiltering = TFO_ANISOTROPIC;
			mAniso = 8;
			break;
		case TFO_ANISOTROPIC:
			mFiltering = TFO_BILINEAR;
			mAniso = 1;
			break;
		default:
			break;
		}
		MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
		MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);


		showDebugOverlay(mStatsOn);

		mTimeUntilNextToggle = 1;
	}

	if (mInputDevice->isKeyDown(KC_SYSRQ) && mTimeUntilNextToggle <= 0)
	{
		char tmp[20];
		sprintf(tmp, "screenshot_%d.png", ++mNumScreenShots);
		mWindow->writeContentsToFile(tmp);
		mTimeUntilNextToggle = 0.5;
		mWindow->setDebugText(String("Wrote ") + tmp);
	}

	if (mInputDevice->isKeyDown(KC_R) && mTimeUntilNextToggle <=0)
	{
		mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
		switch(mSceneDetailIndex) {
				case 0 : mCamera->setDetailLevel(SDL_SOLID) ; break ;
				case 1 : mCamera->setDetailLevel(SDL_WIREFRAME) ; break ;
				case 2 : mCamera->setDetailLevel(SDL_POINTS) ; break ;
		}
		mTimeUntilNextToggle = 0.5;
	}

	static bool displayCameraDetails = false;
	if (mInputDevice->isKeyDown(KC_P) && mTimeUntilNextToggle <= 0)
	{
		displayCameraDetails = !displayCameraDetails;
		mTimeUntilNextToggle = 0.5;
		if (!displayCameraDetails)
			mWindow->setDebugText("");
	}
	if (displayCameraDetails)
	{
		// Print camera details
		mWindow->setDebugText("P: " + StringConverter::toString(mCamera->getDerivedPosition()) + " " +
			"O: " + StringConverter::toString(mCamera->getDerivedOrientation()));
	}

	// Return true to continue rendering
	return true;
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
bool OgreOpcodeExample::processUnbufferedMouseInput(const FrameEvent& evt)
{
	/* Rotation factors, may not be used if the second mouse button is pressed. */

	/* If the second mouse button is pressed, then the mouse movement results in
	sliding the camera, otherwise we rotate. */
	if( mInputDevice->getMouseButton( 1 ) )
	{
		mTranslateVector.x += mInputDevice->getMouseRelativeX() * 0.13;
		mTranslateVector.y -= mInputDevice->getMouseRelativeY() * 0.13;
	}
	else
	{
		mRotX = Degree(-mInputDevice->getMouseRelativeX() * 0.13);
		mRotY = Degree(-mInputDevice->getMouseRelativeY() * 0.13);
	}


	return true;
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::moveCamera()
{

	// Make all the changes to the camera
	// Note that YAW direction is around a fixed axis (freelook style) rather than a natural YAW (e.g. airplane)
	mCamera->yaw(mRotX);
	mCamera->pitch(mRotY);
	mCamera->moveRelative(mTranslateVector);


}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::showDebugOverlay(bool show)
{
	if (mDebugOverlay)
	{
		if (show)
		{
			mDebugOverlay->show();
		}
		else
		{
			mDebugOverlay->hide();
		}
	}
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
bool OgreOpcodeExample::frameStarted(const FrameEvent& evt)
{
	if(mWindow->isClosed())
		return false;

	if (!mInputTypeSwitchingOn)
	{
		mInputDevice->capture();
	}


	if ( !mUseBufferedInputMouse || !mUseBufferedInputKeys)
	{
		// one of the input modes is immediate, so setup what is needed for immediate mouse/key movement
		if (mTimeUntilNextToggle >= 0)
			mTimeUntilNextToggle -= evt.timeSinceLastFrame;

		// If this is the first frame, pick a speed
		if (evt.timeSinceLastFrame == 0)
		{
			mMoveScale = 1;
			mRotScale = 0.1;
		}
		// Otherwise scale movement units by time passed since last frame
		else
		{
			// Move about 100 units per second,
			mMoveScale = mMoveSpeed * evt.timeSinceLastFrame;
			// Take about 10 seconds for full rotation
			mRotScale = mRotateSpeed * evt.timeSinceLastFrame;
		}
		mRotX = 0;
		mRotY = 0;
		mTranslateVector = Vector3::ZERO;
	}

	if (mUseBufferedInputKeys)
	{
		// no need to do any processing here, it is handled by event processor and
		// you get the results as KeyEvents
	}
	else
	{
		if (processUnbufferedKeyInput(evt) == false)
		{
			return false;
		}
	}
	if (mUseBufferedInputMouse)
	{
		// no need to do any processing here, it is handled by event processor and
		// you get the results as MouseEvents
	}
	else
	{
		if (processUnbufferedMouseInput(evt) == false)
		{
			return false;
		}
	}

	if ( !mUseBufferedInputMouse || !mUseBufferedInputKeys)
	{
		// one of the input modes is immediate, so update the movement vector

		moveCamera();

	}

	return true;
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
bool OgreOpcodeExample::frameEnded(const FrameEvent& evt)
{
	updateStats();
	return true;
}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::switchMouseMode()
{
	mUseBufferedInputMouse = !mUseBufferedInputMouse;
	mInputDevice->setBufferedInput(mUseBufferedInputKeys, mUseBufferedInputMouse);
}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::switchKeyMode()
{
	mUseBufferedInputKeys = !mUseBufferedInputKeys;
	mInputDevice->setBufferedInput(mUseBufferedInputKeys, mUseBufferedInputMouse);
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::keyClicked(KeyEvent* e)
{
	if (e->getKeyChar() == 'm')
	{
		switchMouseMode();
	}
	else if (e->getKeyChar() == 'k')
	{

		switchKeyMode();
	}

}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::keyPressed(KeyEvent* e) {}
//-------------------------------------------------------------------------------------
void OgreOpcodeExample::keyReleased(KeyEvent* e) {}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
