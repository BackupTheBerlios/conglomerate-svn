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

[!if MIN_APP]
#include <Ogre.h>
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
   INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
   int main(int argc, char *argv[])
#endif
   {
      // Create application object
      //[!output PROJECT_NAME]App app;

      SET_TERM_HANDLER;

      try {
         //app.go();
      } catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
         MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
         std::cerr << "An exception has occured: " <<
            e.getFullDescription().c_str() << std::endl;
#endif
      }

      return 0;
   }

#ifdef __cplusplus
}
#endif

[!else]

#include "[!output PROJECT_NAME].h"


[!if FRAMEWORK_OWN]
//-------------------------------------------------------------------------------------
[!output PROJECT_NAME]App::[!output PROJECT_NAME]App(void)
{
}
//-------------------------------------------------------------------------------------
[!output PROJECT_NAME]App::~[!output PROJECT_NAME]App(void)
{
}

//-------------------------------------------------------------------------------------
void [!output PROJECT_NAME]App::createScene(void)
{
[!if CEGUI_YES]
	// setup GUI system
	mGUIRenderer = new CEGUI::OgreCEGUIRenderer(mWindow,
	 Ogre::RENDER_QUEUE_OVERLAY, false, 3000);

	mGUISystem = new CEGUI::System(mGUIRenderer);

	CEGUI::Logger::getSingleton().setLoggingLevel(CEGUI::Informative);


	// load scheme and set up defaults
	CEGUI::SchemeManager::getSingleton().loadScheme(
	 (CEGUI::utf8*)"TaharezLook.scheme");
	mGUISystem->setDefaultMouseCursor(
	 (CEGUI::utf8*)"TaharezLook", (CEGUI::utf8*)"MouseArrow");
	mGUISystem->setDefaultFont((CEGUI::utf8*)"Tahoma-12");
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

[!endif]


#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
		int main(int argc, char *argv[])
#endif
		{
			// Create application object
			[!output PROJECT_NAME]App app;

			SET_TERM_HANDLER;

			try {
				app.go();
			} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
				MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
				std::cerr << "An exception has occured: " <<
					e.getFullDescription().c_str() << std::endl;
#endif
			}

			return 0;
		}

#ifdef __cplusplus
	}
#endif
[!endif]
