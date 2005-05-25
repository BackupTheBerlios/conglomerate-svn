#pragma warning( disable : 4267 4311 4244 4312 4530)
#include <State.h>

State::State()
:mInit(false),
mQuit(false)
{
	
}
State::~State(void)
{
}

void State::setupSystem(SceneManager* sm, CEGUI::Renderer* r, CEGUI::System* sys)
{
	gGUISystem = sys;
	gGUIRenderer = r;
	gSceneMgr = sm;
	gStateMgr = StateManager::getSingletonPtr();
}

CEGUI::MouseButton State::convertOgreButtonToCegui(int buttonID)
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


void State::initialize()
{
	mInit = true;
}


bool State::isInitialized()
{
	return mInit;
}

//Static Member Variables
StateManager* State::gStateMgr = NULL;
SceneManager* State::gSceneMgr = NULL;
CEGUI::Renderer * State::gGUIRenderer = NULL;
CEGUI::System* State::gGUISystem = NULL;
