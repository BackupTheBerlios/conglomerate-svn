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
Filename:    ogreOpcodeBSPExample.h
Description: A place for me to try out stuff with OGRE.
-----------------------------------------------------------------------------
*/
#ifndef __ogreOpcodeBSPExample_h__
#define __ogreOpcodeBSPExample_h__

// Include OgreOpcode headers.
#include "OgreOpcode.h"

using namespace OgreOpcode;

#include "OgreOpcodeExample.h"

using namespace Ogre;

class OgreOpcodeBSPExample : public OgreOpcodeExample
{
public:
	OgreOpcodeBSPExample(void);
	virtual ~OgreOpcodeBSPExample(void);

protected:
	virtual void createScene(void);
	virtual bool processUnbufferedKeyInput(const FrameEvent& evt);
	virtual bool frameStarted(const FrameEvent& evt);
private:
	bool mVisualizeObjects;
	bool mDoABBVisualization;
	bool mDoLocalVisualization;
	bool mDoGlobalVisualization;
	Overlay* TargetSight;
	Overlay* hotTargetSight;
	CollisionContext* collideContext;
	CollisionObject* mRobotCollObj;
	MeshCollisionShape* mRobotCollShape;
	Ray ray;
	bool mPlayAnimation;
	String mDbgMsg;
};

#endif // #ifndef __ogreOpcodeBSPExample_h__
