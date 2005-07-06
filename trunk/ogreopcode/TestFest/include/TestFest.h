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
Filename:    TestFest.h
Description: I will implement testing of OgreOpcode with the use of TUT.
-----------------------------------------------------------------------------
*/
#ifndef __TestFest_h_
#define __TestFest_h_

#include "ExampleApplication.h"


class TestFestFrameListener : public ExampleFrameListener
{
private:
   SceneManager* mSceneMgr;
public:
      TestFestFrameListener(SceneManager *sceneMgr, RenderWindow* win, Camera* cam)
         : ExampleFrameListener(win, cam),
         mSceneMgr(sceneMgr)
	{
	}

	bool frameStarted(const FrameEvent& evt)
	{
				bool ret = ExampleFrameListener::frameStarted(evt);
		
      return ret;

	}

};



	class TestFestApp : public ExampleApplication
	{
	public:
		TestFestApp()
      {}

	~TestFestApp()
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
	}

   // Create new frame listener
	void createFrameListener(void)
	{
      mFrameListener= new TestFestFrameListener(mSceneMgr, mWindow, mCamera);
		mRoot->addFrameListener(mFrameListener);
	}
};

#endif // #ifndef __TestFest_h_