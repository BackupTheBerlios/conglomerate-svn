#include "ogreOpcodeDotsceneExample.h"

//-------------------------------------------------------------------------------------
OgreOpcodeDotsceneExample::OgreOpcodeDotsceneExample(void)
: OgreOpcodeExample(),
collideContext(0),
mRobotCollObj(0),
mPlayAnimation(false),
mVisualizeObjects(false),
mDoABBVisualization(false),
mDoLocalVisualization(true),
mDoGlobalVisualization(true)
{
}

//-------------------------------------------------------------------------------------
OgreOpcodeDotsceneExample::~OgreOpcodeDotsceneExample(void)
{
	delete CollisionManager::getSingletonPtr();
}

//-------------------------------------------------------------------------------------
void OgreOpcodeDotsceneExample::createScene(void)
{
	// Set ambient light
	mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));

	// Create a skydome
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	TargetSight = (Overlay*)OverlayManager::getSingleton().getByName("gunTarget");
	TargetSight->show();
	hotTargetSight = (Overlay*)OverlayManager::getSingleton().getByName("hotGunTarget");
	hotTargetSight->hide();

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

	collideContext = CollisionManager::getSingletonPtr()->getDefaultContext();

	Entity* theRobot = mSceneMgr->createEntity("theRobot", "robot.mesh");
	SceneNode* theRobotNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("theRobotNode");
	theRobotNode->attachObject(theRobot);
	theRobotNode->scale(1.8f, 1.8f, 1.8f);

	mRobotCollShape = CollisionManager::getSingletonPtr()->createMeshCollisionShape("ogrehead1");
	mRobotCollShape->load(theRobot);
	mRobotCollObj = collideContext->newObject();
	mRobotCollObj->setCollClass("ogrerobot");
	mRobotCollObj->setShape(mRobotCollShape);
	collideContext->addObject(mRobotCollObj);

	collideContext->reset();
}
//-------------------------------------------------------------------------------------
bool OgreOpcodeDotsceneExample::processUnbufferedKeyInput(const FrameEvent& evt)
{
	if (mInputDevice->isKeyDown(KC_V) && mTimeUntilNextToggle <=0)
	{
		mVisualizeObjects = !mVisualizeObjects;
		mTimeUntilNextToggle = 0.5;
	}

	if (mInputDevice->isKeyDown(KC_L) && mTimeUntilNextToggle <= 0)
	{
		mPlayAnimation = !mPlayAnimation;
		mSceneMgr->getEntity("theRobot")->getAnimationState("Walk")->setEnabled(true);
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

	return OgreOpcodeExample::processUnbufferedKeyInput(evt);
}
//-------------------------------------------------------------------------------------
bool OgreOpcodeDotsceneExample::frameStarted(const FrameEvent& evt)
{
	if (mPlayAnimation)
	{
		mSceneMgr->getEntity("theRobot")->getAnimationState("Walk")->addTime(evt.timeSinceLastFrame/5);
		mRobotCollObj->refit();
	}

	// This has to be here - debug visualization needs to be updated each frame..
	// but only after we update objects!
	collideContext->visualize(mVisualizeObjects, mDoABBVisualization, mDoLocalVisualization, mDoGlobalVisualization);

	CollisionManager::getSingletonPtr()->getDefaultContext()->collide();

	ray = mCamera->getCameraToViewportRay(0.5, 0.5);

	// Do ray testing against everything but the level
	CollisionPair **pick_report;
	int num_picks = CollisionManager::getSingletonPtr()->getDefaultContext()->rayCheck(ray, 2000.0f, COLLTYPE_CONTACT, COLLTYPE_ALWAYS_CONTACT, pick_report);
	const CollisionReporter &rayrept =
		CollisionManager::getSingletonPtr()->getDefaultContext()->getCheckReport();

			mDbgMsg = "";
	if (num_picks > 0)
	{
		TargetSight->hide();
		hotTargetSight->show();
		mDbgMsg += Ogre::StringConverter::toString(num_picks) + " ";
		for(int i = 0; i < num_picks; i++)
		{
			CollisionObject* yeah = pick_report[i]->co1;
			Vector3 contact = pick_report[i]->contact;
			mDbgMsg = mDbgMsg + yeah->getShape()->getName() + " Distance: " + StringConverter::toString(pick_report[i]->distance);
		}
	}
	else
	{
		TargetSight->show();
		hotTargetSight->hide();
	}
	
	mWindow->setDebugText(mDbgMsg);

	return OgreOpcodeExample::frameStarted(evt);
}
