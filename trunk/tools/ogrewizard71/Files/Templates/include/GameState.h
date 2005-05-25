#pragma warning( disable : 4267 )
#ifndef __GameState_H__
#define __GameState_H__

#include "State.h"
#include "Walker.h"
#include <map>
#include <string>

using namespace std;

class GameState : public State
{
public:
	//Constructor
	GameState();
	virtual ~GameState();

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

	//Add waypoint to the current set of waypoints
	void addWaypoint(string who, Vector3 wp);

	std::map<string, Walker*> mModelMap;  //A collection of characters active in the scene

private:
	bool handleQuit(const CEGUI::EventArgs& e);
};
#endif


