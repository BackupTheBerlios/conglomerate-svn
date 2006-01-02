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
Filename:    ogreOpcodeExample.h
Description: A place for me to try out stuff with OGRE.
-----------------------------------------------------------------------------
*/
#ifndef __ogreOpcodeExample_h_
#define __ogreOpcodeExample_h_

// Include OgreOpcode headers.
#include "OgreOpcode.h"

using namespace OgreOpcode;

#include "ExampleApplication.h"
#include "OgreNoMemoryMacros.h"
#include <CEGUI.h>
#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <OgreCEGUIRenderer.h>
#include "OgreMemoryMacros.h"

inline Quaternion makeRandomRotation()
{
  //  From
  //  J. Arvo.
  //  Fast random rotation matices.
  //  In D. Kirk, editor, Graphics Gems III, pages 117-120. Academic Press, 1992.
  Vector3 r(Math::UnitRandom(),Math::UnitRandom(),Math::UnitRandom());
  Real s1r = sqrt(Real(1.0)-r.x);
  Real sr  = sqrt(r.x);
  return Quaternion(
    s1r * sin(Math::TWO_PI*r.y),
    s1r * cos(Math::TWO_PI*r.y),
    sr  * sin(Math::TWO_PI*r.z),
    sr  * cos(Math::TWO_PI*r.z)
    );
}


CEGUI::MouseButton convertOgreButtonToCegui(int buttonID)
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


class ogreOpcodeExampleFrameListener : public ExampleFrameListener, public MouseMotionListener, public MouseListener
{
private:
   SceneManager* mSceneMgr;
   CEGUI::Renderer* mGUIRenderer;
   bool mShutdownRequested;
   bool mVisualizeObjects;
   OgreOpcode::Details::SphereDebugObject* camVisualizer;
   SceneNode* camVisualizerSceneNode;
   Overlay* TargetSight;
   Overlay* hotTargetSight;
   CollisionObject* mCollObj1;
   CollisionObject* mCollObj2;
   CollisionObject* mCollObj3;
   SceneNode* mCamNode;
   Ray ray;
   bool mPlayAnimation;
   String mDbgMsg;
public:
	ogreOpcodeExampleFrameListener(SceneManager *sceneMgr, RenderWindow* win, Camera* cam, CEGUI::Renderer* renderer, CollisionObject* collObj1, CollisionObject* collObj2, CollisionObject* collObj3, SceneNode* camNode)
		: ExampleFrameListener(win, cam, false, true),
      mGUIRenderer(renderer),
      mShutdownRequested(false),
      mVisualizeObjects(0),
      mSceneMgr(sceneMgr),
      mCollObj1(collObj1),
      mCollObj2(collObj2),
      mCollObj3(collObj3),
      mCamNode(camNode),
      mPlayAnimation(true)
   {
      mEventProcessor->addMouseMotionListener(this);
      mEventProcessor->addMouseListener(this);
      mEventProcessor->addKeyListener(this);

      mMoveSpeed = 300;

      TargetSight = (Overlay*)OverlayManager::getSingleton().getByName("gunTarget");
      TargetSight->show();
      hotTargetSight = (Overlay*)OverlayManager::getSingleton().getByName("hotGunTarget");
      hotTargetSight->hide();

      // Move the debug overlay up a bit so we can get 2 lines in.
      OverlayElement* dbgTxt = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
      dbgTxt->setTop(dbgTxt->getTop()-dbgTxt->getHeight());

      switchMouseMode();
   }

   Real IntersectRayPlane(Vector3 rOrigin, Vector3 rVector, Vector3 pOrigin, Vector3 pNormal)
   {

      Real d = - pNormal.dotProduct(pOrigin);

      Real numer = pNormal.dotProduct(rOrigin) + d;
      Real denom = pNormal.dotProduct(rVector);

      if (denom == 0)  // normal is orthogonal to vector, cant intersect
         return (-1.0f);

      return -(numer / denom);
   }

   void SetLength(Vector3& v, Real l)
   {
      Real len = Math::Sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
      v.x *= l/len;
      v.y *= l/len;
      v.z *= l/len;
   }

   Vector3 CheckCollision(const Vector3& pos, float radius, const Vector3& vel)
   {
      static int recursionDepth = 0;         // keeping track of recursions
      static Vector3 lastknownGood(0,0,0);   // last safe position if everything else fails

      // Collision results
      CollisionPair **ray_pick_report;
      CollisionPair **pick_report;

      // The position to test against
      Vector3 destinationPoint = pos + vel;
      Vector3 new_vel(0,0,0);
      int numCamColls = 0;

      // First, construct a ray along the velocity vector and test for collisions.
      Ray testRay(pos, vel);
      numCamColls = CollisionManager::getSingletonPtr()->GetDefaultContext()->RayCheck(testRay, 600.0f, COLLTYPE_CONTACT, COLLTYPE_ALWAYS_CONTACT, ray_pick_report);
      if (numCamColls == 0)
      {
         // This means that we are more than 600 units away from any potential collisions
         // update last known good position, reset recursion count and return original velocity
         lastknownGood = pos + vel;
         recursionDepth = 0;
         return vel;
      }

      // Okay, we are maybe hitting something here..
      // Measure the distance to the nearest possible collision
      Real distToPlaneIntersection = IntersectRayPlane(ray_pick_report[0]->contact, vel, pos, ray_pick_report[0]->co2_normal);
      // Construct a Vector3 and set its length to that distance
      Vector3 adjVel = vel;
      SetLength(adjVel, distToPlaneIntersection);
      // If we are further away than the nearest possible collision, we have travelled too far
      if(vel.length() > (adjVel.length() + 0.05f))
         destinationPoint = pos + adjVel;   // modify the destination point if we are behind it.


      // Now - perform the real collision detection - this time with a sphere.
      Ogre::Sphere cameraSphere = Ogre::Sphere(destinationPoint, radius);
      numCamColls = 0;
      numCamColls = CollisionManager::getSingletonPtr()->GetDefaultContext()->SphereCheck(cameraSphere, COLLTYPE_EXACT, COLLTYPE_ALWAYS_EXACT, pick_report);

      if (numCamColls == 0)
      {
         // Nope. We are not hitting anything. Update last known good, reset recursion counter and return the velocity.
         lastknownGood = pos + vel;
         recursionDepth = 0;
         return vel;
      }


      // We are hitting something!
      for(int i = 0; i < numCamColls; ++i)
      {
         // Adjust the velocity to prevent the camera from passing through walls, etc.
         Plane collPlane(pick_report[i]->co2_normal, pick_report[i]->contact);
         new_vel += pick_report[i]->co2_normal * ( - pick_report[i]->co2_normal.dotProduct(destinationPoint) - (collPlane.d - radius));

         if(recursionDepth > 50)
         {
            // Shit. We are stuck. Update the camera to match the last known good position and return nothing.
            recursionDepth = 0;
            mCamera->setPosition(lastknownGood);
            return Vector3(0,0,0);
         }
      }

      // Don't recurse if the velocity vector is small enough
      if (new_vel.length() < 0.001f)
      {
         recursionDepth = 0;
         return new_vel;
      }

      // Recurse
      recursionDepth++;
      return CheckCollision(pos, radius, new_vel);
   }

   void moveCamera()
   {
      Vector3 camPosition = mCamera->getDerivedPosition();
      Real camRadius = 38.0f;
      Vector3 amount2Move(mTranslateVector);

      // Yep, pressing V will also turn off crappy camera collision handling ..
      if(!mVisualizeObjects)
         amount2Move = CheckCollision(camPosition, camRadius, mTranslateVector);

      mCamera->yaw(mRotX);
      mCamera->pitch(mRotY);
      mCamera->moveRelative(amount2Move);

      // Apply some gravity in a second collision detection pass.
      amount2Move = Vector3(0, -0.89f, 0);
      camPosition = mCamera->getDerivedPosition();
      if(!mVisualizeObjects)
      {
         amount2Move = CheckCollision(camPosition, camRadius, amount2Move);
         mCamera->moveRelative(amount2Move);
      }

   }

   bool frameStarted(const FrameEvent& evt)
	{
      if (!ExampleFrameListener::frameStarted(evt))
        return false;

      CollisionManager::getSingletonPtr()->GetDefaultContext()->Collide();

      static Real transAmount = -0.5f;
      static Real transTraveled = 0.0f;
      if(transTraveled >= 480.0f)
         transAmount = -0.5f;
      if(transTraveled <= -480.0f)
         transAmount = 0.5f;
      transTraveled += transAmount;

      mSceneMgr->getSceneNode("cammnode")->translate(0.0f, transAmount, 0.0f);

      if (mPlayAnimation) {
        mSceneMgr->getEntity("Head1")->getAnimationState("Walk")->addTime(evt.timeSinceLastFrame/5);
        mCollObj2->Refit();
      }
      // This has to be here - debug visualization needs to be updated each frame..
      // but only after we update objects!
      mCollObj1->setDebug(mVisualizeObjects&1);
      mCollObj2->setDebug(mVisualizeObjects);


      if(!mUseBufferedInputMouse)
        ray = mCamera->getCameraToViewportRay(0.5, 0.5);

      // remove level from context - we don't care when ray testing against entities..
      CollisionManager::getSingletonPtr()->GetDefaultContext()->RemoveObject(mCollObj1);

      // Do ray testing against everything but the level
      CollisionPair **pick_report;
      int num_picks = CollisionManager::getSingletonPtr()->GetDefaultContext()->RayCheck(ray, 600.0f, COLLTYPE_EXACT, COLLTYPE_ALWAYS_EXACT, pick_report);
      const CollisionReporter &rayrept =
        CollisionManager::getSingletonPtr()->GetDefaultContext()->GetCheckReport();

      if (num_picks > 0)
      {
         TargetSight->hide();
         hotTargetSight->show();
         mDbgMsg += Ogre::StringConverter::toString(num_picks) + " ";
         for(int i = 0; i < num_picks; i++)
         {
            mDbgMsg = "";
            CollisionObject* yeah = pick_report[i]->co1;
            Vector3 contact = pick_report[i]->contact;
            mDbgMsg = mDbgMsg + yeah->GetShape()->GetName() + " Distance: " + StringConverter::toString(pick_report[i]->distance);
         }
      }
      else
      {
         TargetSight->show();
         hotTargetSight->hide();
      }

      // Done ray testing - Now add the level back into context
      CollisionManager::getSingletonPtr()->GetDefaultContext()->AddObject(mCollObj1);

      // Check all collision objects for collision
      int mCollObj1Picks = mCollObj1->GetCollisions(pick_report);
      if(mCollObj1Picks > 0)
      {
         transAmount = 0.5f;
         mDbgMsg = "Level encountered " + Ogre::StringConverter::toString(mCollObj1Picks) + " collisions";
      }
      const CollisionReporter &rept =
        CollisionManager::getSingletonPtr()->GetDefaultContext()->GetCollisionReport();
      String dbg = mDbgMsg;
      dbg +=
        "\nRayOb: "+ StringConverter::toString(rayrept.mTotalObjObjTests) +
        " RayBV: "+ StringConverter::toString(rayrept.mTotalBVBVTests) +
        " RayPr: "+ StringConverter::toString(rayrept.mTotalBVPrimTests);
      dbg +=
        "\nObOb: "+ StringConverter::toString(rept.mTotalObjObjTests) +
        " BVBV: "+ StringConverter::toString(rept.mTotalBVBVTests) +
        " BVPr: "+ StringConverter::toString(rept.mTotalBVPrimTests) +
        " PrPr: "+ StringConverter::toString(rept.mTotalPrimPrimTests);
      mWindow->setDebugText(dbg);


      if(mUseBufferedInputMouse)
      {
         CEGUI::MouseCursor::getSingleton().show( );
      }
      else
      {
         CEGUI::MouseCursor::getSingleton().hide( );
      }

      return true;
	}

   bool processUnbufferedKeyInput(const FrameEvent& evt)
   {
      if (evt.timeSinceLastFrame == 0.0f) return true;

      if (mInputDevice->isKeyDown(KC_A))
      {
         // Move camera left
         mTranslateVector.x = -mMoveScale;
      }

      if (mInputDevice->isKeyDown(KC_D))
      {
         // Move camera RIGHT
         mTranslateVector.x = mMoveScale;
      }

      /* Move camera forward by keypress. */
      if (mInputDevice->isKeyDown(KC_UP) || mInputDevice->isKeyDown(KC_W) )
      {
         mTranslateVector.z = -mMoveScale;
      }

      /* Move camera backward by keypress. */
      if (mInputDevice->isKeyDown(KC_DOWN) || mInputDevice->isKeyDown(KC_S) )
      {
         mTranslateVector.z = mMoveScale;
      }

      if (mInputDevice->isKeyDown(KC_PGUP))
      {
         // Move camera up
         mTranslateVector.y = mMoveScale;
      }

      if (mInputDevice->isKeyDown(KC_PGDOWN))
      {
         // Move camera down
         mTranslateVector.y = -mMoveScale;
      }

      if (mInputDevice->isKeyDown(KC_RIGHT))
      {
         mCamera->yaw(-mRotScale);
      }

      if (mInputDevice->isKeyDown(KC_LEFT))
      {
         mCamera->yaw(mRotScale);
      }

      if( mInputDevice->isKeyDown( KC_ESCAPE) )
      {
         return false;
      }

      // see if switching is on, and you want to toggle
      if (mInputTypeSwitchingOn && mInputDevice->isKeyDown(KC_M) && mTimeUntilNextToggle <= 0)
      {
         switchMouseMode();
         mTimeUntilNextToggle = 1;
      }

      if (mInputTypeSwitchingOn && mInputDevice->isKeyDown(KC_K) && mTimeUntilNextToggle <= 0)
      {
         // must be going from immediate keyboard to buffered keyboard
         switchKeyMode();
         mTimeUntilNextToggle = 1;
      }
      if (mInputDevice->isKeyDown(KC_F) && mTimeUntilNextToggle <= 0)
      {
         mStatsOn = !mStatsOn;
         showDebugOverlay(mStatsOn);

         mTimeUntilNextToggle = 1;
      }
      if (mInputDevice->isKeyDown(KC_T) && mTimeUntilNextToggle <= 0)
      {
         switch(mFiltering)
         {
         case TFO_BILINEAR:
            mFiltering = TFO_TRILINEAR;
            mAniso = 1;
            break;
         case TFO_TRILINEAR:
            mFiltering = TFO_ANISOTROPIC;
            mAniso = 8;
            break;
         case TFO_ANISOTROPIC:
            mFiltering = TFO_BILINEAR;
            mAniso = 1;
            break;
         default:
            break;
         }
         MaterialManager::getSingleton().setDefaultTextureFiltering(mFiltering);
         MaterialManager::getSingleton().setDefaultAnisotropy(mAniso);


         showDebugOverlay(mStatsOn);

         mTimeUntilNextToggle = 1;
      }

      if (mInputDevice->isKeyDown(KC_SYSRQ) && mTimeUntilNextToggle <= 0)
      {
         char tmp[20];
         sprintf(tmp, "screenshot_%d.png", ++mNumScreenShots);
         mWindow->writeContentsToFile(tmp);
         mTimeUntilNextToggle = 0.5;
         mWindow->setDebugText(String("Wrote ") + tmp);
      }

      if (mInputDevice->isKeyDown(KC_R) && mTimeUntilNextToggle <=0)
      {
         mSceneDetailIndex = (mSceneDetailIndex+1)%3 ;
         switch(mSceneDetailIndex) {
            case 0 : mCamera->setDetailLevel(SDL_SOLID) ; break ;
            case 1 : mCamera->setDetailLevel(SDL_WIREFRAME) ; break ;
            case 2 : mCamera->setDetailLevel(SDL_POINTS) ; break ;
         }
         mTimeUntilNextToggle = 0.5;
      }

      static bool displayCameraDetails = false;
      if (mInputDevice->isKeyDown(KC_P) && mTimeUntilNextToggle <= 0)
      {
         displayCameraDetails = !displayCameraDetails;
         mTimeUntilNextToggle = 0.5;
         if (!displayCameraDetails)
            mWindow->setDebugText("");
      }
      if (displayCameraDetails)
      {
         // Print camera details
         mWindow->setDebugText("P: " + StringConverter::toString(mCamera->getDerivedPosition()) + " " +
            "O: " + StringConverter::toString(mCamera->getDerivedOrientation()));
      }

      if (mInputDevice->isKeyDown(KC_V) && mTimeUntilNextToggle <=0)
      {
         mVisualizeObjects = !mVisualizeObjects;
         mTimeUntilNextToggle = 0.5;
      }

      if (mInputDevice->isKeyDown(KC_SPACE) && mTimeUntilNextToggle <=0)
      {
         mCamera->moveRelative(Vector3(0, 3.0f, 0));
         //mTimeUntilNextToggle = 0.5;
      }

      if (mInputDevice->isKeyDown(KC_L) && mTimeUntilNextToggle <= 0)
      {
        mPlayAnimation = !mPlayAnimation;
        mSceneMgr->getEntity("Head1")->getAnimationState("Walk")->setEnabled(true);
        mTimeUntilNextToggle = 0.5;
      }


      if (mInputDevice->isKeyDown(KC_BACKSLASH) && mTimeUntilNextToggle <=0)
      {
        Quaternion q = makeRandomRotation();
        mSceneMgr->getEntity("Head1")->getParentNode()->setOrientation(q);
        mTimeUntilNextToggle = 0.5;
      }


      // Return true to continue rendering
      return true;
   }

   void requestShutdown(void)
   {
      mShutdownRequested = true;
   }

   bool frameEnded(const FrameEvent& evt)
   {
      if (mShutdownRequested)
         return false;
      else
                  return ExampleFrameListener::frameEnded(evt);
            }

   void mouseMoved (MouseEvent *e)
   {
      CEGUI::System::getSingleton().injectMouseMove(
         e->getRelX() * mGUIRenderer->getWidth(),
         e->getRelY() * mGUIRenderer->getHeight());
      e->consume();
   }

   void mouseDragged (MouseEvent *e)
   {
      mouseMoved(e);
   }

   void mousePressed (MouseEvent *e)
   {
      CEGUI::System::getSingleton().injectMouseButtonDown(
         convertOgreButtonToCegui(e->getButtonID()));
      e->consume();
   }

   void mouseReleased (MouseEvent *e)
   {
      CEGUI::System::getSingleton().injectMouseButtonUp(
         convertOgreButtonToCegui(e->getButtonID()));
      e->consume();
   }

   void mouseClicked(MouseEvent* e)
   {
      ray = mCamera->getCameraToViewportRay(e->getX(), e->getY());
   }
   void mouseEntered(MouseEvent* e) {}
   void mouseExited(MouseEvent* e) {}

   void keyPressed(KeyEvent* e)
   {
      if(e->getKey() == KC_ESCAPE)
      {
         mShutdownRequested = true;
         e->consume();
         return;
      }
      if(e->getKey() == KC_SPACE)
      {
         mVisualizeObjects = true;
         e->consume();
         return;
      }

      CEGUI::System::getSingleton().injectKeyDown(e->getKey());
      CEGUI::System::getSingleton().injectChar(e->getKeyChar());
      e->consume();
   }
};



	class ogreOpcodeExampleApp : public ExampleApplication
	{
   private:
      CEGUI::OgreCEGUIRenderer* mGUIRenderer;
      CEGUI::System* mGUISystem;
      CollisionContext* collideContext;
      CollisionObject *collideObject;
      CollisionObject *collideObject1;
      CollisionObject *collideObject2;
      SceneNode* camNode;
	public:
		ogreOpcodeExampleApp()
         : mGUIRenderer(0),
         mGUISystem(0),
         collideContext(0)
      {}

	~ogreOpcodeExampleApp()
	{
      if(mGUISystem)
      {
         delete mGUISystem;
         mGUISystem = 0;
      }
      if(mGUIRenderer)
      {
         delete mGUIRenderer;
         mGUIRenderer = 0;
      }
      delete CollisionManager::getSingletonPtr();
	}

protected:

	virtual void createCamera(void)
	{
      // Create the camera
      mCamera = mSceneMgr->createCamera("PlayerCam");
      mCamera->setPosition(-7.1680, 292.723, 354.041);
      mCamera->setOrientation(Quaternion(0.998007, 0.0623525, 0.00879721, 0.000549518));
      mCamera->setNearClipDistance(5);
	}

   virtual bool setup(void)
   {
      mRoot = new Root();

      setupResources();

      bool carryOn = configure();
      if (!carryOn) return false;

      chooseSceneManager();
      createCamera();
      createViewports();

      // Set default mipmap level (NB some APIs ignore this)
      TextureManager::getSingleton().setDefaultNumMipmaps(5);

      // Create any resource listeners (for loading screens)
      createResourceListener();
      // Load resources
      loadResources();

      // Create the scene
      createScene();

      createFrameListener();

      return true;

   }

   virtual bool configure(void)
   {
      // Show the configuration dialog and initialise the system
      // You can skip this and use root.restoreConfig() to load configuration
      // settings if you were sure there are valid ones saved in ogre.cfg
      if(mRoot->restoreConfig() || mRoot->showConfigDialog())
      {
         // If returned true, user clicked OK so initialise
         // Here we choose to let the system create a default rendering window by passing 'true'
         mWindow = mRoot->initialise(true);
         return true;
      }
      else
      {
         return false;
      }
   }


	// Just override the mandatory create scene method
	virtual void createScene(void)
	{
      // Should work with or without shadows on now.
      //mSceneMgr->setShadowTechnique(SHADOWTYPE_STENCIL_ADDITIVE);

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


      new CollisionManager(mSceneMgr);
      CollisionManager::getSingletonPtr()->BeginCollClasses();
      CollisionManager::getSingletonPtr()->AddCollClass("level");
      CollisionManager::getSingletonPtr()->AddCollClass("bullet");
      CollisionManager::getSingletonPtr()->AddCollClass("ogrehead");
      CollisionManager::getSingletonPtr()->AddCollClass("powerup");
      CollisionManager::getSingletonPtr()->EndCollClasses();

      CollisionManager::getSingletonPtr()->BeginCollTypes();
      CollisionManager::getSingletonPtr()->AddCollType("level", "level", COLLTYPE_EXACT);
      CollisionManager::getSingletonPtr()->AddCollType("ogrehead", "bullet", COLLTYPE_EXACT);
      CollisionManager::getSingletonPtr()->AddCollType("level", "ogrehead", COLLTYPE_EXACT);
      CollisionManager::getSingletonPtr()->AddCollType("level", "powerup", COLLTYPE_QUICK);
      CollisionManager::getSingletonPtr()->AddCollType("level", "ogrehead", COLLTYPE_EXACT);
      CollisionManager::getSingletonPtr()->AddCollType("powerup", "powerup", COLLTYPE_IGNORE);
      CollisionManager::getSingletonPtr()->AddCollType("powerup", "bullet", COLLTYPE_IGNORE);
      CollisionManager::getSingletonPtr()->AddCollType("bullet", "bullet", COLLTYPE_IGNORE);
      CollisionManager::getSingletonPtr()->EndCollTypes();

      collideContext = CollisionManager::getSingletonPtr()->GetDefaultContext();

      camNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("cameraSceneNode");
      //camNode->attachObject(mCamera);

      Entity* ogreCam = mSceneMgr->createEntity("Head1", "robot.mesh");
      //Entity* ogreCam = mSceneMgr->createEntity("Head1", "ogrehead.mesh");
      SceneNode* tcamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("cammnode");
      //LogManager::getSingleton().logMessage("Walk enabled initially: " + StringConverter::toString(ogreCam->getAnimationState("Walk")->getEnabled()));
      tcamNode->attachObject(ogreCam);
      tcamNode->scale(1.8f, 1.8f, 1.8f);
      tcamNode->setPosition(0.0f,400.0f,0.0f);

      Entity* markerEnt = mSceneMgr->createEntity("markerr", "cube.mesh");
      SceneNode* markerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("markerrnode");
      markerNode->scale(0.001f,0.001f,0.001f);
      markerNode->attachObject(markerEnt);
      markerNode->setPosition(25.3666f, 26.7621f, -24.636);

      Entity* ogreLevel = mSceneMgr->createEntity("Level", "level.mesh");
      SceneNode* levelNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Headnode");
      levelNode->attachObject(ogreLevel);
      //levelNode->scale(1.8f, 1.8f, 1.8f);
      //levelNode->rotate(Vector3::UNIT_X,Degree(-90.0f));

      CollisionShape *collideShape = CollisionManager::getSingletonPtr()->NewShape("level1");
      collideShape->Load(ogreLevel);
      collideObject = collideContext->NewObject();
      collideObject->SetCollClass("level");
      collideObject->SetShape(collideShape);
      collideContext->AddObject(collideObject);

      CollisionShape *collideShape1 = CollisionManager::getSingletonPtr()->NewShape("ogrehead1");
      collideShape1->Load(ogreCam);
      collideObject1 = collideContext->NewObject();
      collideObject1->SetCollClass("ogrehead");
      collideObject1->SetShape(collideShape1);
      collideContext->AddObject(collideObject1);

      collideContext->Reset();

      // Set ambient light
      mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

      // Create a light
      Light* l = mSceneMgr->createLight("MainLight");
      l->setPosition(20,80,50);
      l->setCastShadows(false);

      Light* l2 = mSceneMgr->createLight("ShadowLight");
      l2->setPosition(20,550,350);
      l2->setCastShadows(true);
	}

   // Create new frame listener
	void createFrameListener(void)
	{
		mFrameListener= new ogreOpcodeExampleFrameListener(mSceneMgr, mWindow, mCamera, mGUIRenderer, collideObject, collideObject1, collideObject2, camNode);
		mRoot->addFrameListener(mFrameListener);
	}
   void setupEventHandlers(void)
   {
   }

   bool handleQuit(const CEGUI::EventArgs& e)
   {
      static_cast<ogreOpcodeExampleFrameListener*>(mFrameListener)->requestShutdown();
      return true;
   }
};

#endif // #ifndef __ogreOpcodeExample_h_