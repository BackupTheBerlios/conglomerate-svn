#pragma warning( disable : 4267 4311 4244 4312 4530)

#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/CEGUIImageset.h>
#include <OgreCEGUIRenderer.h>

#include <string>
#include <GameState.h>

using namespace std;

//Constructor
GameState::GameState()
:State()
{
}
GameState::~GameState()
{
}

void GameState::initialize()
{
	mInit = true;
	mQuit = false;

	//Set the default lighting
	State::gSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

	//	 Create objects so we can see movement
	Entity *ent;
	SceneNode *node;
	ent = State::gSceneMgr->createEntity( "Knot1", "knot.mesh" );
	node = State::gSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot1Node",
		Vector3(  0.0f, -10.0f,  25.0f ) );
	node->attachObject( ent );
	node->setScale( 0.1f, 0.1f, 0.1f );
	ent = State::gSceneMgr->createEntity( "Knot2", "knot.mesh" );
	node = State::gSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot2Node",
		Vector3( 550.0f, -10.0f,  50.0f ) );
	node->attachObject( ent );
	node->setScale( 0.1f, 0.1f, 0.1f );
	ent = State::gSceneMgr->createEntity( "Knot3", "knot.mesh" );
	node = State::gSceneMgr->getRootSceneNode( )->createChildSceneNode( "Knot3Node",
		Vector3(-100.0f, -10.0f,-200.0f ) );
	node->attachObject( ent );
	node->setScale( 0.1f, 0.1f, 0.1f );

	Camera * cam = State::gSceneMgr->getCamera("PlayerCam");
	
		// Set the camera to look at our handywork
	//cam->setPosition( 90.0f, 280.0f, 535.0f );
	//cam->pitch( Degree(-40.0f) );
	//cam->yaw( Degree(-15.0f) );

	std::deque<Vector3> walklist;   // initialize robots to stand still
	Entity* entity = State::gSceneMgr->createEntity( "Robot1", "robot.mesh" );
	node = State::gSceneMgr->getRootSceneNode( )->createChildSceneNode( "RobotNode1", Vector3( 0.0f, 0.0f, 25.0f ) );
	node->attachObject(entity);
	mModelMap["Robot1"] = new Walker(node, entity, walklist);
	Walker* w = mModelMap["Robot1"];
	w->setSpeed(50.0f);

	entity = State::gSceneMgr->createEntity( "Robot2", "robot.mesh" );
	node = State::gSceneMgr->getRootSceneNode( )->createChildSceneNode( "RobotNode2", Vector3( 0.0f, 0.0f, 25.0f ) );
	node->attachObject(entity);
	mModelMap["Robot2"] = new Walker(node, entity, walklist);

	addWaypoint("Robot1", Vector3( 550.0f, 0.0f, 50.0f ));
	addWaypoint("Robot1", Vector3( -100.0f, 0.0f, -200.0f ));
	addWaypoint("Robot1", Vector3( 0.0f, 0.0f, 25.0f ));
	addWaypoint("Robot2", Vector3( 550.0f, 0.0f, 50.0f ));
	addWaypoint("Robot2", Vector3( -100.0f, 0.0f, -200.0f ));
	addWaypoint("Robot2", Vector3( 0.0f, 0.0f, 25.0f ));

	setupEventHandlers();
}
void GameState::cleanup()
{
	mInit = false;
	mQuit = false;

	State::gSceneMgr->clearScene();

	for (std::map<string, Walker*>::iterator it = mModelMap.begin(); it != mModelMap.end(); it++)
	{
		delete(it->second);
	}
	
	mModelMap.clear();

}

void GameState::resume()
{
	//Set the default lighting
	State::gSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));
}

bool GameState::frameStarted(Real dt)
{

	if (mQuit)
	{
		State::gStateMgr->changeState(MAINMENU);
	}
	else
	{
		for (std::map<string, Walker*>::iterator it = mModelMap.begin(); it != mModelMap.end(); it++)
		{
			it->second->frameStarted(dt);
		}
	}

	return false;
}
bool GameState::frameEnded(Real dt)
{
	if (mQuit)
	{
		State::gStateMgr->changeState(MAINMENU);
	}

	return false;
}

//Input functions
void GameState::mouseMoved (MouseEvent *e)
{
	State::gGUISystem->injectMouseMove(
		e->getRelX() * State::gGUIRenderer->getWidth(), 
		e->getRelY() * State::gGUIRenderer->getHeight());
	e->consume();
}
void GameState::mouseDragged (MouseEvent *e)
{
	mouseMoved(e);
}
void GameState::mousePressed (MouseEvent *e)
{
	CEGUI::System::getSingleton().injectMouseButtonDown(
		State::convertOgreButtonToCegui(e->getButtonID()));
	e->consume();
}
void GameState::mouseReleased (MouseEvent *e)
{
	CEGUI::System::getSingleton().injectMouseButtonUp(
		State::convertOgreButtonToCegui(e->getButtonID()));
	e->consume();
}
void GameState::keyPressed(KeyEvent* e)
{
	if (e->getKey() == KC_ESCAPE)
	{
		mQuit = true;
		e->consume();
		return;
	}
	State::gGUISystem->injectKeyDown(e->getKey());
	State::gGUISystem->injectChar(e->getKeyChar());
	e->consume();
}

void GameState::setupEventHandlers(void)
{
}

void GameState::addWaypoint(string who, Vector3 wp)
{
	std::map<string, Walker*>::iterator it = mModelMap.find(who);
	if (it != mModelMap.end())
	{
		it->second->addWaypoint(wp);
	}
}

bool GameState::handleQuit(const CEGUI::EventArgs& e)
{
	mQuit = true;
	return true;
}

