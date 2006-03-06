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
Filename:    ogreOpcodeExample.h
Description: A place for me to try out stuff with OGRE.
-----------------------------------------------------------------------------
*/
#ifndef __ogreOpcodeExampleApp_h_
#define __ogreOpcodeExampleApp_h_

// Include OgreOpcode headers.
#include "OgreOpcode.h"

using namespace OgreOpcode;

#include "OgreOpcodeExample.h"

inline Quaternion makeRandomRotation()
{
	//  From
	//  J. Arvo.
	//  Fast random rotation matices.
	//  In D. Kirk, editor, Graphics Gems III, pages 117-120. Academic Press, 1992.
	Vector3 r(Math::UnitRandom(),Math::UnitRandom(),Math::UnitRandom());
	Real s1r = sqrt(Real(1.0)-r.x);
	Real sr  = sqrt(r.x);
	return Quaternion(
		s1r * sin(Math::TWO_PI*r.y),
		s1r * cos(Math::TWO_PI*r.y),
		sr  * sin(Math::TWO_PI*r.z),
		sr  * cos(Math::TWO_PI*r.z)
		);
}

using namespace Ogre;

class GameEntity
{
  protected:
	SceneManager* mSceneMgr;
	SceneNode* mSceneNode;
	CollisionContext* mCollideContext;
	CollisionObject* mCollObj;
    Entity* mEntity;

  public:
    GameEntity(SceneManager* sm) 
      : mSceneMgr(sm),
	    mSceneNode(0), 
        mEntity(0) { }

    virtual ~GameEntity() { }

    virtual bool initialise(void) = 0;

    void setPosition(const Vector3& v)
    {
      mSceneNode->setPosition(v);
    } 
    
    void setPosition(Real x, Real y, Real z)
    {
      mSceneNode->setPosition(Vector3(x, y, z));
    }
    
	CollisionObject* getCollisionObject(void)
    {
      return mCollObj;
    }
	
	bool hasCollisions()
	{
		return mCollObj->hasCollisions();
	}

	int getCollisions(CollisionPair **&crp)
	{
		return mCollObj->getCollisions(crp);
	};
};

class RobotEntity : public GameEntity
{
protected:
	MeshCollisionShape* mRobotCollShape;
public:
    RobotEntity(SceneManager* sm)
      : GameEntity(sm) { }
    ~RobotEntity() { }

	bool reinit(void)
	{
		static int count = 0;

		mRobotCollShape = CollisionManager::getSingletonPtr()->createMeshCollisionShape("ogrehead1" + StringConverter::toString(count));
		mRobotCollShape->load(mEntity);
		mCollObj = mCollideContext->newObject("ogrerobot" + StringConverter::toString(count));
		mCollObj->setCollClass("ogrerobot");
		mCollObj->setShape(mRobotCollShape);
		mCollideContext->addObject(mCollObj);
		
		count++;

		return true;
	}

	bool initialise(void)
    {
		mCollideContext = CollisionManager::getSingletonPtr()->getDefaultContext();
		mEntity = mSceneMgr->createEntity("theRobot", "ninja.mesh");
		mSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("theRobotNode");
		mSceneNode->attachObject(mEntity);
		mSceneNode->setPosition(0.0f, 400.0f, -240.0f);
		mSceneNode->scale(0.6f, 0.6f, 0.6f);
		mEntity->setNormaliseNormals(true);

	  return reinit();
    }

    void translate(const Vector3& v)
    {
      mSceneNode->translate(v);
    }

    void translate(Real x, Real y, Real z)
    {
      mSceneNode->translate(x, y, z);
    }

	MeshCollisionShape* getCollisionShape(void)
	{
		return mRobotCollShape;
	}
};

class OgreOpcodeExampleApp : public OgreOpcodeExample
{
public:
	OgreOpcodeExampleApp(void);
	virtual ~OgreOpcodeExampleApp(void);

protected:
	virtual void createScene(void);
	virtual bool processUnbufferedKeyInput(const FrameEvent& evt);
	virtual bool frameStarted(const FrameEvent& evt);
private:
	void parseDotScene( const String& SceneName);
	void addCollisionShape( const String& shapeName, Entity* entity, bool makeStatic);

	bool mVisualizeObjects;
	bool mDoABBVisualization;
	bool mDoLocalVisualization;
	bool mDoGlobalVisualization;
	Overlay* mTargetSight;
	Overlay* mHotTargetSight;
	Overlay* mInfoOverlay;
	OverlayElement* mRayObjectText;
	OverlayElement* mRayDistanceText;
	CollisionContext* mCollideContext;
	CollisionObject* mRobotCollObj;
	CollisionObject* mTestCollObj;
	MeshCollisionShape* mRobotCollShape;
	SphereMeshCollisionShape* mTestCollShape;
	Ray mRay;
	bool mPlayAnimation;
	String mDbgMsg1;
	String mDbgMsg2;
	RobotEntity* mRobotEntity;
};

#endif // #ifndef __ogreOpcodeExampleApp_h_
