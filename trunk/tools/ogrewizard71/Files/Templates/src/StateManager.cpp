#pragma warning( disable : 4267 4311 4244 4312 4530)
#include <StateManager.h>
#include <State.h>
#include <MainMenuState.h>
#include <GameState.h>

// Singleton instance
template<> StateManager* Singleton<StateManager>::ms_Singleton = 0;

StateManager::StateManager()
:mGUIRenderer(0),
mSM(0)
{
	mpCurrentState = NULL;
	
}
StateManager::~StateManager()
{
	if (!mStateMap.empty())
	{
		for (std::map<STATES,State*>::iterator i=mStateMap.begin();
			i != mStateMap.end(); ++i)
		{
			delete i->second;
		}
	}
}

void StateManager::changeState(STATES next_state)
{
	if (mpCurrentState)
		mpCurrentState->cleanup();

	std::map<STATES,State*>::iterator it = mStateMap.find(next_state);
	if (it == mStateMap.end())
	{
		switch(next_state)
		{
		case MAINMENU:
			mStateMap[MAINMENU] = new MainMenuState();
			break;
		case GAME:
			mStateMap[GAME] = new GameState();
			break;
		default:
			next_state = MAINMENU;
			break;
		}
		
		it = mStateMap.find(next_state);
	}
	
	mpCurrentState = it->second;
	
	if (!mpCurrentState->isInitialized())
	{
		mpCurrentState->initialize();
	}
	else
	{
		mpCurrentState->resume();
	}
}

//Passed on to states

void StateManager::mouseMoved (MouseEvent *e)
{
	mpCurrentState->mouseMoved(e);
}
void StateManager::mouseDragged (MouseEvent *e) 
{
	mpCurrentState->mouseDragged(e);
}
void StateManager::mousePressed (MouseEvent *e)
{
	mpCurrentState->mousePressed(e);
}
void StateManager::mouseReleased (MouseEvent *e)
{
	mpCurrentState->mouseReleased(e);
}
void StateManager::mouseClicked(MouseEvent* e)
{
	mpCurrentState->mouseClicked(e);
}
void StateManager::mouseEntered(MouseEvent* e)
{
	mpCurrentState->mouseEntered(e);
}
void StateManager::mouseExited(MouseEvent* e)
{
	mpCurrentState->mouseExited(e);
}

void StateManager::keyPressed(KeyEvent* e)
{
	mpCurrentState->keyPressed(e);
}

//Return true if application should quit.
bool StateManager::frameStarted(Real dt)
{
	return mpCurrentState->frameStarted(dt);
}
bool StateManager::frameEnded(Real dt)
{
	return mpCurrentState->frameEnded(dt);
}

void StateManager::setGraphicsWorld(SceneManager* sm, CEGUI::Renderer* renderer, CEGUI::System* gui_system)
{
	mSM = sm;
	mGUIRenderer = renderer;
	mGUISystem = gui_system;

	State::setupSystem(sm, renderer, gui_system);

	changeState(MAINMENU);
}