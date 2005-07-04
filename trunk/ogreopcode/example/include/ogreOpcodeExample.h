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

#include "ExampleApplication.h"
#include <OgreNoMemoryMacros.h>
#include <CEGUI.h>
#include <CEGUISystem.h>
#include <CEGUISchemeManager.h>
#include <OgreCEGUIRenderer.h>
#include <OgreMemoryMacros.h>

using namespace OgreOpcode;

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
   Ogre::Ray line;
public:
   ogreOpcodeExampleFrameListener(SceneManager *sceneMgr, RenderWindow* win, Camera* cam, CEGUI::Renderer* renderer, CollisionObject* collObj1, CollisionObject* collObj2, CollisionObject* collObj3, SceneNode* camNode)
      : ExampleFrameListener(win, cam, false, true),
      mGUIRenderer(renderer),
      mShutdownRequested(false),
      mVisualizeObjects(false),
      mSceneMgr(sceneMgr),
      mCollObj1(collObj1),
      mCollObj2(collObj2),
      mCollObj3(collObj3),
      mCamNode(camNode)
   {
      mEventProcessor->addMouseMotionListener(this);
      mEventProcessor->addMouseListener(this);
      mEventProcessor->addKeyListener(this);

      mMoveSpeed = 400;

      TargetSight = (Overlay*)OverlayManager::getSingleton().getByName("gunTarget");    
      TargetSight->show();
      hotTargetSight = (Overlay*)OverlayManager::getSingleton().getByName("hotGunTarget");    
      hotTargetSight->hide();

      //      camVisualizer = new Ogre::Debug::SphereDebugObject(20.0f);
      //      camVisualizerSceneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("camVizNode");
      //      camVisualizerSceneNode->attachObject(camVisualizer);
      //      camVisualizer->draw();
      switchMouseMode();
   }

   Real Distance(Vector3 vPoint1, Vector3 vPoint2)
   {
      // This is the classic formula used in beginning algebra to return the
      // distance between 2 points.  Since it's 3D, we just add the z dimension:
      // 
      // Distance = sqrt(  (P2.x - P1.x)^2 + (P2.y - P1.y)^2 + (P2.z - P1.z)^2 )
      //
      Real distance = Math::Sqrt( (vPoint2.x - vPoint1.x) * (vPoint2.x - vPoint1.x) +
         (vPoint2.y - vPoint1.y) * (vPoint2.y - vPoint1.y) +
         (vPoint2.z - vPoint1.z) * (vPoint2.z - vPoint1.z) );

      // Return the distance between the 2 points
      return distance;
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

   Vector3 PushAwayFrom(const Vector3& V, const Vector3& position, const Real radius)
   {
      //--------------------------------------------------------------------------
      // calculate the separation vector
      //--------------------------------------------------------------------------
      Vector3 D = position - V;
      float  d = D.length();

      //--------------------------------------------------------------------------
      // calculate the depth of intersection
      //--------------------------------------------------------------------------
      Real depth = radius - d;

      //--------------------------------------------------------------------------
      // push the sphere along the separation vector, by the intersection depth amount
      //--------------------------------------------------------------------------
      Vector3 ret(position);
      ret += D * (depth / d);
      return ret;
   }

   void doCollisions(const Vector3& velocity, const Vector3& position)
   {
      static int num_recurs = 0;
      Vector3 new_velocity(0,0,0);
      if(num_recurs > 200)
      {
         num_recurs = 0;
         mCamera->setPosition(position);
         return;
      }
      Vector3 destinationPoint = position + velocity;
      Vector3 new_Destination;
      Vector3 newBasePoint = position;
      CollisionPair **pick_report;
      int num_picks = 0;
      Real radius = 10.0f;
      Ogre::Sphere cameraSphere(destinationPoint, radius);
      num_picks = CollisionManager::getSingletonPtr()->GetDefaultContext()->SphereCheck(cameraSphere, COLLTYPE_EXACT, COLLTYPE_ALWAYS_EXACT, pick_report);
      if(num_picks > 0)
      {
         for(int i = 0; i < num_picks; ++i)
         {
            Vector3 intersectPt = pick_report[i]->contact;
            Vector3 intersectNl = pick_report[i]->co2_normal;            
            Real distToPlaneIntersection = IntersectRayPlane(intersectPt, velocity, position, intersectNl);
            mWindow->setDebugText("Collide distance = " + Ogre::StringConverter::toString(distToPlaneIntersection));

            Vector3 newSourcePoint;

            // only update if we are not already very close
            if (distToPlaneIntersection >= radius)
            {

               Vector3 V = velocity.normalisedCopy();
               SetLength(V, distToPlaneIntersection - radius);
               newSourcePoint.x = position.x + V.x;
               newSourcePoint.y = position.y + V.y;
               newSourcePoint.z = position.z + V.z;
            }
            else if (distToPlaneIntersection < 0.0f)
            {
               Vector3 V = -velocity.normalisedCopy();
               SetLength(V, distToPlaneIntersection - radius);
               newSourcePoint.x = position.x + V.x;
               newSourcePoint.y = position.y + V.y;
               newSourcePoint.z = position.z + V.z;
            }
            else
               newSourcePoint = position;

            // Determine the sliding plane (we do this now, because we're about to
            // change sourcePoint)
            Vector3 slidePlaneOrigin = intersectPt;
            Vector3 slidePlaneNormal;
            slidePlaneNormal.x = newSourcePoint.x - intersectPt.x;
            slidePlaneNormal.y = newSourcePoint.y - intersectPt.y;
            slidePlaneNormal.z = newSourcePoint.z - intersectPt.z;
            // We now project the destination point onto the sliding plane
            Real l = IntersectRayPlane(position, slidePlaneNormal, slidePlaneOrigin, slidePlaneNormal);
            Vector3 newDestinationPoint;
            newDestinationPoint.x = position.x + l * slidePlaneNormal.x;
            newDestinationPoint.y = position.y + l * slidePlaneNormal.y;
            newDestinationPoint.z = position.z + l * slidePlaneNormal.z;

            Vector3 newVelocityVector;
            newVelocityVector.x = newDestinationPoint.x - intersectPt.x;
            newVelocityVector.y = newDestinationPoint.y - intersectPt.y;
            newVelocityVector.z = newDestinationPoint.z - intersectPt.z;

            newBasePoint = destinationPoint;
            new_velocity = newVelocityVector;
            mCamera->moveRelative(new_velocity);
         }
         num_recurs++;
         //         new_velocity /= num_picks;
         doCollisions(new_velocity, newBasePoint);
      }
      if(num_picks == 0)
      {
         num_recurs = 0;
         mCamera->moveRelative(velocity);
      }
   }

   void moveCamera()
   {
      Real camRadius = 18.0f;

      mCamera->yaw(mRotX);
      mCamera->pitch(mRotY);

      // Yep, pressing SPACEBAR will also turn off crappy camera collision handling ..
      if(!mVisualizeObjects)
      {
         doCollisions(mTranslateVector, mCamera->getDerivedPosition());
      } else
         mCamera->moveRelative(mTranslateVector);
   }

   bool frameStarted(const FrameEvent& evt)
   {
      // This has to be here - debug visualization needs to be updated each frame..
      mCollObj1->setDebug(mVisualizeObjects);
      mCollObj2->setDebug(mVisualizeObjects);

      //mSceneMgr->getSceneNode("camVizNode")->setPosition(mCamera->getDerivedPosition());
      //mSceneMgr->getSceneNode("camVizNode")->setOrientation(mCamera->getDerivedOrientation());

      static float transAmount = -0.5f;
      static float transTraveled = 0.0f;
      if(transTraveled >= 480.0f)
         transAmount = -0.5f;
      if(transTraveled <= -480.0f)
         transAmount = 0.5f;
      transTraveled += transAmount;

      bool ret = ExampleFrameListener::frameStarted(evt);

      if(!mUseBufferedInputMouse)
         line = mCamera->getCameraToViewportRay(0.5, 0.5);

      mSceneMgr->getSceneNode("cammnode")->translate(0.0f, transAmount, 0.0f);

      // remove level from context - we don't care when ray testing against entities..
      CollisionManager::getSingletonPtr()->GetDefaultContext()->RemoveObject(mCollObj1);

      // Do ray testing against everything but the level
      CollisionPair **pick_report;
      int num_picks = CollisionManager::getSingletonPtr()->GetDefaultContext()->LineCheck(line, 600.0f, COLLTYPE_EXACT, COLLTYPE_ALWAYS_EXACT, pick_report);
      String nameStr;

      if (num_picks > 0)
      {
         TargetSight->hide();
         hotTargetSight->show();
         nameStr += Ogre::StringConverter::toString(num_picks) + " ";
         for(int i = 0; i < num_picks; i++)
         {
            nameStr = "";
            CollisionObject* yeah = pick_report[i]->co1;
            Vector3 contact = pick_report[i]->contact;
            nameStr = nameStr + yeah->GetShape()->GetName() + " Contact: " + Ogre::StringConverter::toString(contact);
         }
      }
      if (num_picks == 0)
      {
         TargetSight->show();
         hotTargetSight->hide();
      }

      // Done ray testing - Now add the level back into context
      CollisionManager::getSingletonPtr()->GetDefaultContext()->AddObject(mCollObj1);


      // Check all collision objects for collision
      CollisionManager::getSingletonPtr()->GetDefaultContext()->Collide();
      int mCollObj1Picks = mCollObj1->GetCollissions(pick_report);
      if(mCollObj1Picks > 0)
      {
         transAmount = 0.5f;
         //         nameStr = "Level encountered " + Ogre::StringConverter::toString(mCollObj1Picks) + " collisions";
      }
      // mWindow->setDebugText(nameStr);



      if(mUseBufferedInputMouse)
      {
         CEGUI::MouseCursor::getSingleton().show( );
      }
      else
      {
         CEGUI::MouseCursor::getSingleton().hide( );
      }

      return ret;

   }

   bool processUnbufferedKeyInput(const FrameEvent& evt)
   {
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

      if (mInputDevice->isKeyDown(KC_SPACE) && mTimeUntilNextToggle <=0)
      {
         mVisualizeObjects = !mVisualizeObjects;
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
      line = mCamera->getCameraToViewportRay(e->getX(), e->getY());
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
      delete mGUISystem;
      mGUISystem = 0;
      delete mGUIRenderer;
      mGUIRenderer = 0;
      delete CollisionManager::getSingletonPtr();
   }

protected:

   virtual void createCamera(void)
   {
      // Create the camera
      mCamera = mSceneMgr->createCamera("PlayerCam");
      mCamera->setPosition(0,0,0);

      // Position it at 500 in Z direction
      // Look back along -Z
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

      //      camNode->setFixedYawAxis(true);
      //      camNode->attachObject(mCamera);
      mCamera->setPosition(-7.1680, 292.723, 354.041);
      mCamera->setOrientation(Quaternion(0.998007, 0.0623525, 0.00879721, 0.000549518));

      Entity* ogreCam = mSceneMgr->createEntity("Head1", "ogrehead.mesh");
      SceneNode* tcamNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("cammnode");
      tcamNode->attachObject(ogreCam);

      Entity* markerEnt = mSceneMgr->createEntity("markerr", "cube.mesh");
      SceneNode* markerNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("markerrnode");
      markerNode->scale(0.001f,0.001f,0.001f);
      markerNode->attachObject(markerEnt);
      markerNode->setPosition(25.3666f, 26.7621f, -24.636);

      Entity* ogreLevel = mSceneMgr->createEntity("Level", "level.mesh");
      SceneNode* levelNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("Headnode");
      levelNode->attachObject(ogreLevel);
      //levelNode->rotate(Vector3::UNIT_X,Degree(-90.0f));

      CollisionShape *collideShape = CollisionManager::getSingletonPtr()->NewShape("level1");
      collideShape->Load(ogreLevel);
      collideObject = collideContext->NewObject();
      collideObject->SetCollClass(CollisionManager::getSingletonPtr()->QueryCollClass("level"));
      collideObject->SetShape(collideShape);
      collideContext->AddObject(collideObject);

      CollisionShape *collideShape1 = CollisionManager::getSingletonPtr()->NewShape("ogrehead1");
      collideShape1->Load(ogreCam);
      collideObject1 = collideContext->NewObject();
      collideObject1->SetCollClass(CollisionManager::getSingletonPtr()->QueryCollClass("ogrehead"));
      collideObject1->SetShape(collideShape1);
      collideContext->AddObject(collideObject1);

      collideContext->Reset();

      // Set ambient light
      mSceneMgr->setAmbientLight(ColourValue(0.5, 0.5, 0.5));

      // Create a light
      Light* l = mSceneMgr->createLight("MainLight");
      l->setPosition(20,80,50);
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