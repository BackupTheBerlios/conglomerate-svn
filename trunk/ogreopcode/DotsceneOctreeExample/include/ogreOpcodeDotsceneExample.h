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
Filename:    ogreOpcodeDotsceneExample.h
Description: A place for me to try out stuff with OGRE.
-----------------------------------------------------------------------------
*/
#ifndef __ogreOpcodeDotsceneExample_h__
#define __ogreOpcodeDotsceneExample_h__

#include "DotSceneManager.h"

// Include OgreOpcode headers.
#include "OgreOpcode.h"

using namespace OgreOpcode;

#include "OgreOpcodeExample.h"

using namespace Ogre;

class OgreOpcodeDotsceneExample : public OgreOpcodeExample
{
public:
	OgreOpcodeDotsceneExample(void);
	virtual ~OgreOpcodeDotsceneExample(void);

protected:
	virtual void createScene(void);
	virtual bool processUnbufferedKeyInput(const FrameEvent& evt);
	virtual bool frameStarted(const FrameEvent& evt);

	virtual void chooseSceneManager(void)
	{
		mSceneMgr = mRoot->createSceneManager("DotSceneOctreeManager");
		mDotSceneMgr = static_cast <DotSceneOctreeManager *> ( mSceneMgr );
	}

private:
	Light* LoadLight( TiXmlElement *XMLLight );
	void parseDotScene( const String &SceneName, const String& groupName );

	// The library is statically linked to give access to extra functions in the DotSceneMgr
	DotSceneOctreeManager *mDotSceneMgr;

	bool mVisualizeObjects;
	bool mDoABBVisualization;
	bool mDoLocalVisualization;
	bool mDoGlobalVisualization;
	Overlay* TargetSight;
	Overlay* hotTargetSight;
	CollisionContext* collideContext;
	CollisionObject* mRobotCollObj;
	EntityCollisionShape* mRobotCollShape;
	PtrCollisionShape* mDotSceneShape;
	Ray ray;
	bool mPlayAnimation;
	bool mShowOctree;
	String mDbgMsg;
};

#endif // #ifndef __ogreOpcodeDotsceneExample_h__
