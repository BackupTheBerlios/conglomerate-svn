#pragma warning( disable : 4267 4311 4244 4312 4530 )

#include <CEGUI/CEGUISchemeManager.h>
#include <CEGUI/CEGUIWindowManager.h>
#include <CEGUI/CEGUIWindow.h>
#include <CEGUI/elements/CEGUIPushButton.h>
#include <CEGUI/elements/CEGUIStaticImage.h>
#include <CEGUI/CEGUITexture.h>
#include <CEGUI/CEGUIImageset.h>
#include <OgreCEGUIRenderer.h>
#include <OgreCEGUIResourceProvider.h>
#include <CEGUI/CEGUIDefaultResourceProvider.h>


#include <MainMenuState.h>


//Constructor
MainMenuState::MainMenuState()
:State()
{

	//Imageset has to be created before the layout is loaded
	CEGUI::Texture* myTexture = State::gGUIRenderer->createTexture("greenbg.jpg", "General");
	CEGUI::Imageset* pImageSet = CEGUI::ImagesetManager::getSingleton().createImageset("background", myTexture);
	pImageSet->defineImage("bg", CEGUI::Point(0.0f, 0.0f),CEGUI::Size(1024,1024),CEGUI::Point(0.0f,0.0f));

	CEGUI::Window* sheet = 
		CEGUI::WindowManager::getSingleton().loadWindowLayout(
		(CEGUI::utf8*)"MainMenu.layout"); 
	State::gGUISystem->setGUISheet(sheet);

		setupEventHandlers();
}
MainMenuState::~MainMenuState()
{

}

void MainMenuState::mouseMoved (MouseEvent *e)
{
	State::gGUISystem->injectMouseMove(
		e->getRelX() * State::gGUIRenderer->getWidth(), 
		e->getRelY() * State::gGUIRenderer->getHeight());
	e->consume();
}
void MainMenuState::keyPressed(KeyEvent* e)
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
void MainMenuState::mouseDragged (MouseEvent *e) 
{
	mouseMoved(e);
}

void MainMenuState::mousePressed (MouseEvent *e)
{
	CEGUI::System::getSingleton().injectMouseButtonDown(
		State::convertOgreButtonToCegui(e->getButtonID()));
	e->consume();
}

void MainMenuState::mouseReleased (MouseEvent *e)
{
	CEGUI::System::getSingleton().injectMouseButtonUp(
		State::convertOgreButtonToCegui(e->getButtonID()));
	e->consume();
}

// State functions
void MainMenuState::initialize()
{
	mInit = true;
	mQuit = false;

	//Set the default lighting
	State::gSceneMgr->setAmbientLight(ColourValue(1.0f, 1.0f, 1.0f));

	CEGUI::Window* win= CEGUI::WindowManager::getSingleton().getWindow("MainMenu");
	win->show();
	State::gGUISystem->setGUISheet(win);

}

void MainMenuState::setupEventHandlers(void)
{
	CEGUI::WindowManager& wmgr = CEGUI::WindowManager::getSingleton();

	wmgr.getWindow((CEGUI::utf8*)"MainMenu/Menu/Exit")
		->subscribeEvent(
		CEGUI::PushButton::EventClicked, 
		CEGUI::Event::Subscriber(&MainMenuState::handleQuit, this));

	wmgr.getWindow((CEGUI::utf8*)"MainMenu/Menu/New")
		->subscribeEvent(
		CEGUI::PushButton::EventClicked, 
		CEGUI::Event::Subscriber(&MainMenuState::handleNew, this));
}

void MainMenuState::cleanup()
{
	mInit = false;
	mQuit = false;

	CEGUI::Window * window = State::gGUISystem->getGUISheet();
	window->hide();

	State::gSceneMgr->clearScene();
}

void MainMenuState::resume()
{

	CEGUI::Window* win= CEGUI::WindowManager::getSingleton().getWindow("MainMenu");
	win->show();
	State::gGUISystem->setGUISheet(win);
}

bool MainMenuState::frameStarted(Real dt)
{
	return mQuit;
}
bool MainMenuState::frameEnded(Real dt)
{
	return mQuit;
}

bool MainMenuState::handleQuit(const CEGUI::EventArgs& e)
{
	mQuit = true;
	return true;
}
bool MainMenuState::handleNew(const CEGUI::EventArgs& e)
{
	State::gStateMgr->changeState(GAME);
	return true;
}
