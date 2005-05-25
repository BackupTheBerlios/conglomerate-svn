#pragma warning( disable : 4267 )
#ifndef __State_H__
#define __State_H__

#include <CEGUI/CEGUISystem.h>
#include <OgreCEGUIRenderer.h>
#include <Ogre.h>
#include <OgreKeyEvent.h>
#include <OgreEventListeners.h>
#include "StateManager.h"

class State
{
public:
	State();
	virtual ~State(void);
	
	//These functions should be overwritten by specialized classes
	virtual void mouseMoved (MouseEvent *e){}
	virtual void mouseDragged (MouseEvent *e) {}
	virtual void mousePressed (MouseEvent *e){}
	virtual void mouseReleased (MouseEvent *e){}
	virtual void mouseClicked(MouseEvent* e){}
	virtual void mouseEntered(MouseEvent* e){}
	virtual void mouseExited(MouseEvent* e){}
	virtual void keyPressed(KeyEvent* e){}

	// State functions
	virtual void initialize();
	virtual void cleanup(){}

	virtual void pause(){}
	virtual void resume(){}

	//These must be overwritten by the specialized class
	virtual bool frameStarted(Real dt) = 0;
	virtual bool frameEnded(Real dt) = 0;

	bool isInitialized();

	//Static functions
	static void setupSystem(SceneManager* sm, CEGUI::Renderer* r, CEGUI::System* sys);
	static CEGUI::MouseButton convertOgreButtonToCegui(int buttonID);

protected:
	static StateManager* gStateMgr;
	static SceneManager* gSceneMgr;
	static CEGUI::Renderer * gGUIRenderer;
	static CEGUI::System* gGUISystem;
	
	bool mQuit;
	bool mInit;
	

};

#endif
