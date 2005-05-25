#pragma warning( disable : 4267 )

#ifndef __MainMenuState_H__
#define __MainMenuState_H__

#include "State.h"


class MainMenuState : public State
{
public:
    //Constructor
	MainMenuState();
	virtual ~MainMenuState();
	
	// State functions
	virtual void initialize();
	virtual void cleanup();
	
	virtual void pause(){}
	virtual void resume();

	virtual bool frameStarted(Real dt);
	virtual bool frameEnded(Real dt);
	
	//Input functions
	virtual void mouseMoved (MouseEvent *e);
	virtual void mouseDragged (MouseEvent *e);
	virtual void mousePressed (MouseEvent *e);
	virtual void mouseReleased (MouseEvent *e);
	virtual void keyPressed(KeyEvent* e);

protected:
	virtual void setupEventHandlers(void);
	
private:
	bool handleQuit(const CEGUI::EventArgs& e);
	bool handleNew(const CEGUI::EventArgs& e);
};
#endif