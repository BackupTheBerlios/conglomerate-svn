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
Filename:    BaseApplication.h
Description: A place for me to try out stuff with OGRE.
-----------------------------------------------------------------------------
*/
#ifndef __BaseApplication_h_
#define __BaseApplication_h_

#include <ogre.h>
#include <OgreKeyEvent.h>
#include <OgreEventListeners.h>
#include <OgreStringConverter.h>
#include <OgreException.h>

using namespace Ogre;

class LoadingBar;

class BaseApplication : public Ogre::Singleton<BaseApplication>,  public FrameListener, public KeyListener
{
public:
	BaseApplication(void);
	virtual ~BaseApplication(void);

	virtual void go(void);

protected:
	virtual bool setup();
	virtual bool configure(void);
	virtual void chooseSceneManager(void);
	virtual void createCamera(void);
	virtual void createFrameListener(void);
	virtual void createScene(void) = 0; // Override me!
	virtual void destroyScene(void);
	virtual void createViewports(void);
	virtual void setupResources(void);
	virtual void createResourceListener(void);
	virtual void loadResources(void);
	virtual void updateStats(void);
	virtual bool processUnbufferedKeyInput(const FrameEvent& evt);
	virtual bool processUnbufferedMouseInput(const FrameEvent& evt);
	virtual void moveCamera();
	virtual bool frameStarted(const FrameEvent& evt);
	virtual bool frameEnded(const FrameEvent& evt);

	void showDebugOverlay(bool show);
	void switchMouseMode();
	void switchKeyMode();
	void keyClicked(KeyEvent* e);
	void keyPressed(KeyEvent* e);
	void keyReleased(KeyEvent* e);

	Root *mRoot;
	Camera* mCamera;
	SceneManager* mSceneMgr;
	RenderWindow* mWindow;
	LoadingBar* mLoadingBar;
	int mSceneDetailIndex ;
	Real mMoveSpeed;
	Degree mRotateSpeed;
	Overlay* mDebugOverlay;
	EventProcessor* mEventProcessor;
	InputReader* mInputDevice;
	Vector3 mTranslateVector;
	bool mStatsOn;
	bool mUseBufferedInputKeys, mUseBufferedInputMouse, mInputTypeSwitchingOn;
	unsigned int mNumScreenShots;
	float mMoveScale;
	Degree mRotScale;
	Real mTimeUntilNextToggle; // just to stop toggles flipping too fast
	Radian mRotX, mRotY;
	TextureFilterOptions mFiltering;
	int mAniso;
};

#endif // #ifndef __BaseApplication_h_
