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

[!if MIN_APP]
[!else]

[!if FRAMEWORK_OWN]
#include "BaseApplication.h"
[!else]
#include "ExampleApplication.h"
[!endif]


[!if FRAMEWORK_OWN]

class [!output PROJECT_NAME]App : public BaseApplication
{
public:
	[!output PROJECT_NAME]App(void);
	virtual ~[!output PROJECT_NAME]App(void);

protected:
	virtual void createScene(void);
};

[!else]

[!if CEGUI_YES]
#include <OgreNoMemoryMacros.h>
#include <CEGUI.h>
#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <OgreCEGUIRenderer.h>
#include <OgreMemoryMacros.h>

CEGUI::MouseButton convertOgreButtonToCegui(int buttonID)
{
   switch (buttonID)
   {
   case MouseEvent::BUTTON0_MASK:
      return CEGUI::LeftButton;
   case MouseEvent::BUTTON1_MASK:
      return CEGUI::RightButton;
   case MouseEvent::BUTTON2_MASK:
      return CEGUI::MiddleButton;
   case MouseEvent::BUTTON3_MASK:
      return CEGUI::X1Button;
   default:
      return CEGUI::LeftButton;
   }
}
[!endif]


[!if CEGUI_YES]
class [!output PROJECT_NAME]FrameListener : public ExampleFrameListener, public MouseMotionListener, public MouseListener
[!else]
class [!output PROJECT_NAME]FrameListener : public ExampleFrameListener
[!endif]
{
private:
   SceneManager* mSceneMgr;
[!if CEGUI_YES]
   CEGUI::Renderer* mGUIRenderer;
   bool mShutdownRequested;
[!endif]
public:
[!if CEGUI_YES]
	[!output PROJECT_NAME]FrameListener(SceneManager *sceneMgr, RenderWindow* win, Camera* cam, CEGUI::Renderer* renderer)
		: ExampleFrameListener(win, cam, false, true),
      mGUIRenderer(renderer),
      mShutdownRequested(false),
      mSceneMgr(sceneMgr)
[!else]
      [!output PROJECT_NAME]FrameListener(SceneManager *sceneMgr, RenderWindow* win, Camera* cam)
         : ExampleFrameListener(win, cam),
         mSceneMgr(sceneMgr)
[!endif]
	{
[!if CEGUI_YES]
      mEventProcessor->addMouseMotionListener(this);
      mEventProcessor->addMouseListener(this);
      mEventProcessor->addKeyListener(this);
[!endif]
	}

	bool frameStarted(const FrameEvent& evt)
	{
		[!if FRAMEWORK_OWN]
		bool ret = BaseFrameListener::frameStarted(evt);
		[!else]
		bool ret = ExampleFrameListener::frameStarted(evt);
		[!endif]

[!if CEGUI_YES]
      if(mUseBufferedInputMouse)
      {
         CEGUI::MouseCursor::getSingleton().show( );
      }
      else
      {
         CEGUI::MouseCursor::getSingleton().hide( );
      }
[!endif]

      return ret;

	}

[!if CEGUI_YES]
   void requestShutdown(void)
   {
      mShutdownRequested = true;
   }

   bool frameEnded(const FrameEvent& evt)
   {
      if (mShutdownRequested)
         return false;
      else
         [!if FRAMEWORK_OWN]
         return BaseFrameListener::frameEnded(evt);
         [!else]
         return ExampleFrameListener::frameEnded(evt);
         [!endif]
   }

   void mouseMoved (MouseEvent *e)
   {
      CEGUI::System::getSingleton().injectMouseMove(
         e->getRelX() * mGUIRenderer->getWidth(),
         e->getRelY() * mGUIRenderer->getHeight());
      e->consume();
   }

   void mouseDragged (MouseEvent *e)
   {
      mouseMoved(e);
   }

   void mousePressed (MouseEvent *e)
   {
      CEGUI::System::getSingleton().injectMouseButtonDown(
         convertOgreButtonToCegui(e->getButtonID()));
      e->consume();
   }

   void mouseReleased (MouseEvent *e)
   {
      CEGUI::System::getSingleton().injectMouseButtonUp(
         convertOgreButtonToCegui(e->getButtonID()));
      e->consume();
   }

   void mouseClicked(MouseEvent* e) {}
   void mouseEntered(MouseEvent* e) {}
   void mouseExited(MouseEvent* e) {}

   void keyPressed(KeyEvent* e)
   {
      if(e->getKey() == KC_ESCAPE)
      {
         mShutdownRequested = true;
         e->consume();
         return;
      }

      CEGUI::System::getSingleton().injectKeyDown(e->getKey());
      CEGUI::System::getSingleton().injectChar(e->getKeyChar());
      e->consume();
   }
[!endif]
};



class [!output PROJECT_NAME]App : public ExampleApplication
{
[!if CEGUI_YES]
   private:
      CEGUI::OgreCEGUIRenderer* mGUIRenderer;
      CEGUI::System* mGUISystem;
[!endif]
	public:
		[!output PROJECT_NAME]App()
[!if CEGUI_YES]
         : mGUIRenderer(0),
         mGUISystem(0)
[!endif]
      {}

	~[!output PROJECT_NAME]App()
	{
[!if CEGUI_YES]
      if(mGUISystem)
      {
         delete mGUISystem;
         mGUISystem = 0;
      }
      if(mGUIRenderer)
      {
         delete mGUIRenderer;
         mGUIRenderer = 0;
      }
[!endif]
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
[!if CEGUI_YES]
		// setup GUI system
		mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow,
	 Ogre::RENDER_QUEUE_OVERLAY, false, 3000, mSceneMgr);

		mGUISystem = new CEGUI::System(mGUIRenderer);

		CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);


		// load scheme and set up defaults
		CEGUI::SchemeManager::getSingleton().loadScheme(
		 (CEGUI::utf8*)"TaharezLookSkin.scheme");
		mGUISystem->setDefaultMouseCursor(
		 (CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
		mGUISystem->setDefaultFont((CEGUI::utf8*)"BlueHighway-12");
		CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");
		CEGUI::MouseCursor::getSingleton().show( );
		setupEventHandlers();
[!endif]

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
[!if CEGUI_YES]
		mFrameListener= new [!output PROJECT_NAME]FrameListener(mSceneMgr, mWindow, mCamera, mGUIRenderer);
[!else]
      mFrameListener= new [!output PROJECT_NAME]FrameListener(mSceneMgr, mWindow, mCamera);
[!endif]
		mRoot->addFrameListener(mFrameListener);
	}
[!if CEGUI_YES]
   void setupEventHandlers(void)
   {
   }

   bool handleQuit(const CEGUI::EventArgs& e)
   {
      static_cast<[!output PROJECT_NAME]FrameListener*>(mFrameListener)->requestShutdown();
      return true;
   }
[!endif]
};
[!endif]
[!endif]

#endif // #ifndef __[!output PROJECT_NAME]_h_