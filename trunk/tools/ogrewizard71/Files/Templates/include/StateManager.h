#pragma warning( disable : 4267 )

#ifndef __StateManager_H__
#define __StateManager_H__

#include <CEGUI/CEGUISystem.h>
#include <OgreCEGUIRenderer.h>
#include <Ogre.h>
#include <OgreKeyEvent.h>
#include <OgreEventListeners.h>

//forward declarations
class State;

using namespace Ogre;

enum STATES
{
	MAINMENU,
	GAME
};

class StateManager : public Singleton<StateManager>
{
private:
	State* mpCurrentState;
	std::map<STATES,State*> mStateMap;
	

protected:
	
	SceneManager* mSM;
	CEGUI::Renderer* mGUIRenderer;
	CEGUI::System* mGUISystem;

public:
	StateManager();
	~StateManager();

	void changeState(STATES next_state);

	//Passed on to states
	void mouseMoved (MouseEvent *e);
	void mouseDragged (MouseEvent *e) ;
	void mousePressed (MouseEvent *e);
	void mouseReleased (MouseEvent *e);
	void mouseClicked(MouseEvent* e);
	void mouseEntered(MouseEvent* e);
	void mouseExited(MouseEvent* e);

	void keyPressed(KeyEvent* e);

	//Return true if application should quit.
	bool frameStarted(Real dt);
	bool frameEnded(Real dt);
	
	//Initialization - Must be called before any states can be constructed.
	void setGraphicsWorld(SceneManager* sm, CEGUI::Renderer* renderer, CEGUI::System* gui_system);
};

#endif	
