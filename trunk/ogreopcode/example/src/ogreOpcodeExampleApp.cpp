#include "ogreOpcodeExampleApp.h"

#include "NikkiDotscene.h"

using namespace nikkiloader;

//-------------------------------------------------------------------------------------
OgreOpcodeExampleApp::OgreOpcodeExampleApp(void)
: OgreOpcodeExample(),
collideContext(0),
mRobotCollObj(0),
mPlayAnimation(false),
mVisualizeObjects(false),
mDoABBVisualization(false),
mDoLocalVisualization(true),
mDoGlobalVisualization(true)
{
}

//-------------------------------------------------------------------------------------
OgreOpcodeExampleApp::~OgreOpcodeExampleApp(void)
{
	delete CollisionManager::getSingletonPtr();
}

//-------------------------------------------------------------------------------------
void OgreOpcodeExampleApp::createScene(void)
{
	// Set ambient light
	mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));

	// Create a skydome
	mSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);

	TargetSight = (Overlay*)OverlayManager::getSingleton().getByName("gunTarget");
	TargetSight->show();
	hotTargetSight = (Overlay*)OverlayManager::getSingleton().getByName("hotGunTarget");
	hotTargetSight->hide();

	// Move the debug overlay up a bit so we can get 2 lines in.
	OverlayElement* dbgTxt = OverlayManager::getSingleton().getOverlayElement("Core/DebugText");
	dbgTxt->setTop(dbgTxt->getTop()-dbgTxt->getHeight());

	new CollisionManager(mSceneMgr);
	CollisionManager::getSingletonPtr()->beginCollClasses();
	CollisionManager::getSingletonPtr()->addCollClass("level");
	CollisionManager::getSingletonPtr()->addCollClass("bullet");
	CollisionManager::getSingletonPtr()->addCollClass("ogrerobot");
	CollisionManager::getSingletonPtr()->addCollClass("powerup");
	CollisionManager::getSingletonPtr()->endCollClasses();

	CollisionManager::getSingletonPtr()->beginCollTypes();
	CollisionManager::getSingletonPtr()->addCollType("level", "level", COLLTYPE_IGNORE);
	CollisionManager::getSingletonPtr()->addCollType("ogrerobot", "bullet", COLLTYPE_EXACT);
	CollisionManager::getSingletonPtr()->addCollType("level", "ogrerobot", COLLTYPE_EXACT);
	CollisionManager::getSingletonPtr()->addCollType("level", "powerup", COLLTYPE_QUICK);
	CollisionManager::getSingletonPtr()->addCollType("level", "ogrerobot", COLLTYPE_EXACT);
	CollisionManager::getSingletonPtr()->addCollType("powerup", "powerup", COLLTYPE_IGNORE);
	CollisionManager::getSingletonPtr()->addCollType("powerup", "bullet", COLLTYPE_IGNORE);
	CollisionManager::getSingletonPtr()->addCollType("bullet", "bullet", COLLTYPE_IGNORE);
	CollisionManager::getSingletonPtr()->endCollTypes();

	collideContext = CollisionManager::getSingletonPtr()->getDefaultContext();

	Entity* theRobot = mSceneMgr->createEntity("theRobot", "robot.mesh");
	theRobot->setNormaliseNormals(true);
	SceneNode* theRobotNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("theRobotNode");
	theRobotNode->attachObject(theRobot);
	theRobotNode->scale(1.8f, 1.8f, 1.8f);

	mRobotCollShape = static_cast<MeshCollisionShape*>(CollisionManager::getSingletonPtr()->newShape("ogrehead1"));
	mRobotCollShape->load(theRobot);
	mRobotCollObj = collideContext->newObject();
	mRobotCollObj->setCollClass("ogrerobot");
	mRobotCollObj->setShape(mRobotCollShape);
	collideContext->addObject(mRobotCollObj);

	parseDotScene("scene.xml");

	collideContext->reset();
}
//-------------------------------------------------------------------------------------
bool OgreOpcodeExampleApp::processUnbufferedKeyInput(const FrameEvent& evt)
{
	if (mInputDevice->isKeyDown(KC_V) && mTimeUntilNextToggle <=0)
	{
		mVisualizeObjects = !mVisualizeObjects;
		mTimeUntilNextToggle = 0.5;
	}

	if (mInputDevice->isKeyDown(KC_L) && mTimeUntilNextToggle <= 0)
	{
		mPlayAnimation = !mPlayAnimation;
		mSceneMgr->getEntity("theRobot")->getAnimationState("Walk")->setEnabled(true);
		mTimeUntilNextToggle = 0.5;
	}

	if (mInputDevice->isKeyDown(KC_Z) && mTimeUntilNextToggle <= 0)
	{
		mDoABBVisualization = !mDoABBVisualization;
		mTimeUntilNextToggle = 0.5;
	}

	if (mInputDevice->isKeyDown(KC_X) && mTimeUntilNextToggle <= 0)
	{
		mDoLocalVisualization = !mDoLocalVisualization;
		mTimeUntilNextToggle = 0.5;
	}

	if (mInputDevice->isKeyDown(KC_C) && mTimeUntilNextToggle <= 0)
	{
		mDoGlobalVisualization = !mDoGlobalVisualization;
		mTimeUntilNextToggle = 0.5;
	}

	return OgreOpcodeExample::processUnbufferedKeyInput(evt);
}
//-------------------------------------------------------------------------------------
bool OgreOpcodeExampleApp::frameStarted(const FrameEvent& evt)
{
	if (mPlayAnimation)
	{
		mSceneMgr->getEntity("theRobot")->getAnimationState("Walk")->addTime(evt.timeSinceLastFrame/5);
		mRobotCollObj->refit();
	}

	// This has to be here - debug visualization needs to be updated each frame..
	// but only after we update objects!
	collideContext->visualize(mVisualizeObjects, mDoABBVisualization, mDoLocalVisualization, mDoGlobalVisualization);

	CollisionManager::getSingletonPtr()->getDefaultContext()->collide();

	ray = mCamera->getCameraToViewportRay(0.5, 0.5);

	// Do ray testing against everything but the level
	CollisionPair **pick_report;
	int num_picks = CollisionManager::getSingletonPtr()->getDefaultContext()->rayCheck(ray, 2000.0f, COLLTYPE_CONTACT, COLLTYPE_ALWAYS_CONTACT, pick_report);
	const CollisionReporter &rayrept =
		CollisionManager::getSingletonPtr()->getDefaultContext()->getCheckReport();

			mDbgMsg = "";
	if (num_picks > 0)
	{
		TargetSight->hide();
		hotTargetSight->show();
		mDbgMsg += Ogre::StringConverter::toString(num_picks) + " ";
		for(int i = 0; i < num_picks; i++)
		{
			CollisionObject* yeah = pick_report[i]->co1;
			Vector3 contact = pick_report[i]->contact;
			mDbgMsg = mDbgMsg + yeah->getShape()->getName() + " Distance: " + StringConverter::toString(pick_report[i]->distance);
		}
	}
	else
	{
		TargetSight->show();
		hotTargetSight->hide();
	}
	
	mWindow->setDebugText(mDbgMsg);

	return OgreOpcodeExample::frameStarted(evt);
}
//-------------------------------------------------------------------------------------
void OgreOpcodeExampleApp::addCollisionShape(const String& shapeName, Entity* entity, bool makeStatic)
{
	MeshCollisionShape* tempCollShape;
	CollisionObject* tempCollObject;
	tempCollShape = static_cast<MeshCollisionShape*>(CollisionManager::getSingletonPtr()->newShape(shapeName));
	if(makeStatic)
		tempCollShape->setStatic();
	entity->setNormaliseNormals(true);
	tempCollShape->load(entity);
	tempCollObject = collideContext->newObject();
	if(makeStatic)
	{
		tempCollObject->setCollClass("level");
	}
	else
		tempCollObject->setCollClass("ogrerobot");
	tempCollObject->setShape(tempCollShape);
	collideContext->addObject(tempCollObject);
}
//-------------------------------------------------------------------------------------
void OgreOpcodeExampleApp::parseDotScene( const String &SceneName)
{
	TiXmlDocument   *XMLDoc;
	TiXmlElement   *XMLRoot, *XMLNodes;

	try
	{
		DataStreamPtr pStream = ResourceGroupManager::getSingleton().
			openResource( SceneName, "General" );

		String data = pStream->getAsString();
		// Open the .scene File
		XMLDoc = new TiXmlDocument();
		XMLDoc->Parse( data.c_str() );
		pStream->close();
		pStream.setNull();

		if( XMLDoc->Error() )
		{
			//We'll just log, and continue on gracefully
			LogManager::getSingleton().logMessage("[dotSceneLoader] The TiXmlDocument reported an error");
			delete XMLDoc;
			return;
		}
	}
	catch(...)
	{
		//We'll just log, and continue on gracefully
		LogManager::getSingleton().logMessage("[dotSceneLoader] Error creating TiXmlDocument");
		delete XMLDoc;
		return;
	}

	// Validate the File
	XMLRoot = XMLDoc->RootElement();
	if( String( XMLRoot->Value()) != "scene"  ) {
		LogManager::getSingleton().logMessage( "[dotSceneLoader]Error: Invalid .scene File. Missing <scene>" );
		delete XMLDoc;
		return;
	}

	XMLNodes = XMLRoot->FirstChildElement( "nodes" );

	// Read in the scene nodes
	if( XMLNodes )
	{
		TiXmlElement *XMLNode, *XMLPosition, *XMLRotation, *XMLScale,  *XMLEntity, *XMLBillboardSet,  *XMLLight, *XMLUserData;

		XMLNode = XMLNodes->FirstChildElement( "node" );

		while( XMLNode )
		{
			// Process the current node
			// Grab the name of the node
			String NodeName = XMLNode->Attribute("name");
			// First create the new scene node
			SceneNode* NewNode = mSceneMgr->getRootSceneNode()->createChildSceneNode( NodeName );
			Vector3 TempVec;
			String TempValue;

			// Now position it...
			XMLPosition = XMLNode->FirstChildElement("position");
			if( XMLPosition ){
				TempValue = XMLPosition->Attribute("x");
				TempVec.x = StringConverter::parseReal(TempValue);
				TempValue = XMLPosition->Attribute("y");
				TempVec.y = StringConverter::parseReal(TempValue);
				TempValue = XMLPosition->Attribute("z");
				TempVec.z = StringConverter::parseReal(TempValue);
				NewNode->setPosition( TempVec );
			}

			// Rotate it...
			XMLRotation = XMLNode->FirstChildElement("rotation");
			if( XMLRotation ){
				Quaternion TempQuat;
				TempValue = XMLRotation->Attribute("qx");
				TempQuat.x = StringConverter::parseReal(TempValue);
				TempValue = XMLRotation->Attribute("qy");
				TempQuat.y = StringConverter::parseReal(TempValue);
				TempValue = XMLRotation->Attribute("qz");
				TempQuat.z = StringConverter::parseReal(TempValue);
				TempValue = XMLRotation->Attribute("qw");
				TempQuat.w = StringConverter::parseReal(TempValue);
				NewNode->setOrientation( TempQuat );
			}

			// Scale it.
			XMLScale = XMLNode->FirstChildElement("scale");
			if( XMLScale ){
				TempValue = XMLScale->Attribute("x");
				TempVec.x = StringConverter::parseReal(TempValue);
				TempValue = XMLScale->Attribute("y");
				TempVec.y = StringConverter::parseReal(TempValue);
				TempValue = XMLScale->Attribute("z");
				TempVec.z = StringConverter::parseReal(TempValue);
				NewNode->setScale( TempVec );
			}

			XMLLight = XMLNode->FirstChildElement( "light" );
			if( XMLLight )
				NewNode->attachObject( LoadLight( XMLLight, mSceneMgr ) );

			// Check for an Entity
			XMLEntity = XMLNode->FirstChildElement("entity");
			if( XMLEntity )
			{
				String EntityName, EntityMeshFilename, IamStatic;
				EntityName = XMLEntity->Attribute( "name" );
				EntityMeshFilename = XMLEntity->Attribute( "meshFile" );
				IamStatic = XMLEntity->Attribute( "static" );
				bool makeStatic = StringConverter::parseBool(IamStatic);

				// Create entity
				Entity* NewEntity = mSceneMgr->createEntity(EntityName, EntityMeshFilename);
				NewNode->attachObject( NewEntity );

				addCollisionShape(EntityName, NewEntity, makeStatic);

			}

			XMLBillboardSet = XMLNode->FirstChildElement( "billboardSet" );
			if( XMLBillboardSet )
			{
				String TempValue;

				BillboardSet* bSet = mSceneMgr->createBillboardSet( NewNode->getName() );

				BillboardType Type;
				TempValue = XMLBillboardSet->Attribute( "type" );
				if( TempValue == "orientedCommon" )
					Type = BBT_ORIENTED_COMMON;
				else if( TempValue == "orientedSelf" )
					Type = BBT_ORIENTED_SELF;
				else Type = BBT_POINT;

				BillboardOrigin Origin;
				TempValue = XMLBillboardSet->Attribute( "type" );
				if( TempValue == "bottom_left" )
					Origin = BBO_BOTTOM_LEFT;
				else if( TempValue == "bottom_center" )
					Origin = BBO_BOTTOM_CENTER;
				else if( TempValue == "bottomRight"  )
					Origin = BBO_BOTTOM_RIGHT;
				else if( TempValue == "left" )
					Origin = BBO_CENTER_LEFT;
				else if( TempValue == "right" )
					Origin = BBO_CENTER_RIGHT;
				else if( TempValue == "topLeft" )
					Origin = BBO_TOP_LEFT;
				else if( TempValue == "topCenter" )
					Origin = BBO_TOP_CENTER;
				else if( TempValue == "topRight" )
					Origin = BBO_TOP_RIGHT;
				else
					Origin = BBO_CENTER;

				bSet->setBillboardType( Type );
				bSet->setBillboardOrigin( Origin );


				TempValue = XMLBillboardSet->Attribute( "name" );
				bSet->setMaterialName( TempValue );

				int width, height;
				width = (int) StringConverter::parseReal( XMLBillboardSet->Attribute( "width" ) );
				height = (int) StringConverter::parseReal( XMLBillboardSet->Attribute( "height" ) );
				bSet->setDefaultDimensions( width, height );
				bSet->setVisible( true );
				NewNode->attachObject( bSet );

				TiXmlElement *XMLBillboard;

				XMLBillboard = XMLBillboardSet->FirstChildElement( "billboard" );

				while( XMLBillboard )
				{
					Billboard *b;
					// TempValue;
					TempVec = Vector3( 0, 0, 0 );
					ColourValue TempColour(1,1,1,1);

					XMLPosition = XMLBillboard->FirstChildElement( "position" );
					if( XMLPosition ){
						TempValue = XMLPosition->Attribute("x");
						TempVec.x = StringConverter::parseReal(TempValue);
						TempValue = XMLPosition->Attribute("y");
						TempVec.y = StringConverter::parseReal(TempValue);
						TempValue = XMLPosition->Attribute("z");
						TempVec.z = StringConverter::parseReal(TempValue);
					}

					TiXmlElement* XMLColour = XMLBillboard->FirstChildElement( "colourDiffuse" );
					if( XMLColour ){
						TempValue = XMLColour->Attribute("r");
						TempColour.r = StringConverter::parseReal(TempValue);
						TempValue = XMLColour->Attribute("g");
						TempColour.g = StringConverter::parseReal(TempValue);
						TempValue = XMLColour->Attribute("b");
						TempColour.b = StringConverter::parseReal(TempValue);
					}

					b = bSet->createBillboard( TempVec, TempColour);

					XMLBillboard = XMLBillboard->NextSiblingElement( "billboard" );
				}
			}

			XMLUserData = XMLNode->FirstChildElement( "userData" );
			if ( XMLUserData )
			{
				TiXmlElement *XMLProperty;
				XMLProperty = XMLUserData->FirstChildElement("property");
				while ( XMLProperty )
				{
					String first = NewNode->getName();
					String second = XMLProperty->Attribute("name");
					String third = XMLProperty->Attribute("data");
					nodeProperty newProp(first,second,third);
					nodeProperties.push_back(newProp);
					XMLProperty = XMLProperty->NextSiblingElement("property");
				}
			}

			// Move to the next node
			XMLNode = XMLNode->NextSiblingElement( "node" );
		}
	}

	// Close the XML File
	delete XMLDoc;
}
