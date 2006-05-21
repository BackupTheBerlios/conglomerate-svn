///////////////////////////////////////////////////////////////////////////////
///  @file OgreOpcodeDebugObject.cpp
///  @brief <TODO: insert file description here>
///  @remarks  Based directly on code from OgreNewt, by Walaber.
///  @author The OgreOpcode Team
///////////////////////////////////////////////////////////////////////////////
///  
///  This file is part of OgreOpcode.
///  
///  A lot of the code is based on the Nebula Opcode Collision module, see docs/Nebula_license.txt
///  
///  OgreOpcode is free software; you can redistribute it and/or
///  modify it under the terms of the GNU Lesser General Public
///  License as published by the Free Software Foundation; either
///  version 2.1 of the License, or (at your option) any later version.
///  
///  OgreOpcode is distributed in the hope that it will be useful,
///  but WITHOUT ANY WARRANTY; without even the implied warranty of
///  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
///  Lesser General Public License for more details.
///  
///  You should have received a copy of the GNU Lesser General Public
///  License along with OgreOpcode; if not, write to the Free Software
///  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
///  
///////////////////////////////////////////////////////////////////////////////
#include "OgreOpcodeDebugObject.h"

template<> OgreOpcode::Details::OgreOpcodeDebugger* Ogre::Singleton<OgreOpcode::Details::OgreOpcodeDebugger>::ms_Singleton = 0;

namespace OgreOpcode
{
	namespace Details
	{
		OgreOpcodeDebugger* OgreOpcodeDebugger::getSingletonPtr(void)
		{
			return ms_Singleton;
		}

		OgreOpcodeDebugger& OgreOpcodeDebugger::getSingleton(void)
		{  
			assert( ms_Singleton );  return ( *ms_Singleton );  
		}

		OgreOpcodeDebugger::OgreOpcodeDebugger(SceneManager* sceneMgr)
			: mSceneMgr(sceneMgr),
			mRadiiDebugNode(0),
			mContactsDebugNode(0),
			mBBsDebugNode(0),
			mShapesDebugNode(0),
			mAABBsDebugNode(0),
			mRadiiDebugObject(0),
			mContactsDebugObject(0),
			mBBsDebugObject(0),
			mShapesDebugObject(0),
			mAABBsDebugObject(0)
		{
			mRadiiDebugNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("__OgreOpcode__RadiiDebugger__");
			mContactsDebugNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("__OgreOpcode__ContactsDebugger__");
			mBBsDebugNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("__OgreOpcode__BBsDebugger__");
			mShapesDebugNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("__OgreOpcode__ShapesDebugger__");
			mAABBsDebugNode = mSceneMgr->getRootSceneNode()->createChildSceneNode("__OgreOpcode__AABBsDebugger__");

			mRadiiDebugObject = new ManualObject("__OgreOpcode__RadiiDebugger__");
			mContactsDebugObject = new ManualObject("__OgreOpcode__ContactsDebugger__");
			mBBsDebugObject = new ManualObject("__OgreOpcode__BBsDebugger__");
			mShapesDebugObject = new ManualObject("__OgreOpcode__ShapesDebugger__");
			mAABBsDebugObject = new ManualObject("__OgreOpcode__AABBsDebugger__");

			MaterialPtr radiiMat = MaterialManager::getSingleton().create("OgreOpcodeDebug/Radii","OgreOpcode");
			MaterialPtr contactsMat = MaterialManager::getSingleton().create("OgreOpcodeDebug/Contacts","OgreOpcode");
			MaterialPtr BBsMat = MaterialManager::getSingleton().create("OgreOpcodeDebug/BBs","OgreOpcode");
			MaterialPtr shapesMat = MaterialManager::getSingleton().create("OgreOpcodeDebug/Shapes","OgreOpcode");
			MaterialPtr AABBsMat = MaterialManager::getSingleton().create("OgreOpcodeDebug/AABBs","OgreOpcode");

			radiiMat->setReceiveShadows(false);
			radiiMat->getTechnique(0)->setLightingEnabled(true);
			radiiMat->getTechnique(0)->getPass(0)->setDiffuse(0,0,1,0);
			radiiMat->getTechnique(0)->getPass(0)->setAmbient(0,0,1);
			radiiMat->getTechnique(0)->getPass(0)->setSelfIllumination(0,0,1);

			contactsMat->setReceiveShadows(false);
			contactsMat->getTechnique(0)->setLightingEnabled(true);
			contactsMat->getTechnique(0)->getPass(0)->setDiffuse(1,0,0,0);
			contactsMat->getTechnique(0)->getPass(0)->setAmbient(1,0,0);
			contactsMat->getTechnique(0)->getPass(0)->setSelfIllumination(1,0,0);

			BBsMat->setReceiveShadows(false);
			BBsMat->getTechnique(0)->setLightingEnabled(true);
			BBsMat->getTechnique(0)->getPass(0)->setDiffuse(1,1,0,0);
			BBsMat->getTechnique(0)->getPass(0)->setAmbient(1,1,0);
			BBsMat->getTechnique(0)->getPass(0)->setSelfIllumination(1,1,0);

			shapesMat->setReceiveShadows(false);
			shapesMat->getTechnique(0)->setLightingEnabled(true);
			shapesMat->getTechnique(0)->getPass(0)->setDiffuse(0,1,0,0);
			shapesMat->getTechnique(0)->getPass(0)->setAmbient(0,1,0);
			shapesMat->getTechnique(0)->getPass(0)->setSelfIllumination(0,1,0);

			AABBsMat->setReceiveShadows(false);
			AABBsMat->getTechnique(0)->setLightingEnabled(true);
			AABBsMat->getTechnique(0)->getPass(0)->setDiffuse(1,0,1,0);
			AABBsMat->getTechnique(0)->getPass(0)->setAmbient(1,0,1);
			AABBsMat->getTechnique(0)->getPass(0)->setSelfIllumination(1,0,1);
		}

		void OgreOpcodeDebugger::addRadiiLine(Real lx1, Real ly1, Real lz1, Real lx2, Real ly2, Real lz2)
		{
			mRadiiDebugObject->position(lx1, ly1, lz1);
			mRadiiDebugObject->position(lx2, ly2, lz2);
		}
		
		void OgreOpcodeDebugger::addContactLine(Real lx1, Real ly1, Real lz1, Real lx2, Real ly2, Real lz2)
		{
			mContactsDebugObject->position(lx1, ly1, lz1);
			mContactsDebugObject->position(lx2, ly2, lz2);
		}
		
		void OgreOpcodeDebugger::addBBLine(Real lx1, Real ly1, Real lz1, Real lx2, Real ly2, Real lz2)
		{
			mBBsDebugObject->position(lx1, ly1, lz1);
			mBBsDebugObject->position(lx2, ly2, lz2);
		}
		
		void OgreOpcodeDebugger::addShapeLine(Real lx1, Real ly1, Real lz1, Real lx2, Real ly2, Real lz2)
		{
			mShapesDebugObject->position(lx1, ly1, lz1);
			mShapesDebugObject->position(lx2, ly2, lz2);
		}
		
		void OgreOpcodeDebugger::addAABBLine(Real lx1, Real ly1, Real lz1, Real lx2, Real ly2, Real lz2)
		{
			mAABBsDebugObject->position(lx1, ly1, lz1);
			mAABBsDebugObject->position(lx2, ly2, lz2);
		}

		void OgreOpcodeDebugger::clearRadii()
		{
			mRadiiDebugNode->detachAllObjects();
			mRadiiDebugObject->clear();
		}

		void OgreOpcodeDebugger::beginRadii()
		{
			mRadiiDebugObject->begin("OgreOpcodeDebug/Radii", Ogre::RenderOperation::OT_LINE_LIST);
		}
		
		void OgreOpcodeDebugger::endRadii()
		{
			mRadiiDebugObject->end();
			mRadiiDebugNode->attachObject(mRadiiDebugObject); 
		}
		
		void OgreOpcodeDebugger::clearContacts()
		{
			mContactsDebugNode->detachAllObjects();
			mContactsDebugObject->clear();
		}
		
		void OgreOpcodeDebugger::beginContacts()
		{
			mContactsDebugObject->begin("OgreOpcodeDebug/Contacts", Ogre::RenderOperation::OT_LINE_LIST);
		}
		
		void OgreOpcodeDebugger::endContacts()
		{
			mContactsDebugObject->end();
			mContactsDebugNode->attachObject(mContactsDebugObject); 
		}
		
		void OgreOpcodeDebugger::clearBBs()
		{
			mBBsDebugNode->detachAllObjects();
			mBBsDebugObject->clear();
		}

		void OgreOpcodeDebugger::beginBBs()
		{
			mBBsDebugObject->begin("OgreOpcodeDebug/BBs", Ogre::RenderOperation::OT_LINE_LIST);
		}
		
		void OgreOpcodeDebugger::endBBs()
		{
			mBBsDebugObject->end();
			mBBsDebugNode->attachObject(mBBsDebugObject); 
		}
		
		void OgreOpcodeDebugger::clearShapes()
		{
			mShapesDebugNode->detachAllObjects();
			mShapesDebugObject->clear();
		}
		
		void OgreOpcodeDebugger::beginShapes()
		{
			mShapesDebugObject->begin("OgreOpcodeDebug/Shapes", Ogre::RenderOperation::OT_LINE_LIST);
		}

		void OgreOpcodeDebugger::endShapes()
		{
			mShapesDebugObject->end();
			mShapesDebugNode->attachObject(mShapesDebugObject); 
		}
		
		void OgreOpcodeDebugger::clearAABBs()
		{
			mAABBsDebugNode->detachAllObjects();
			mAABBsDebugObject->clear();
		}

		void OgreOpcodeDebugger::beginAABBs()
		{
			mAABBsDebugObject->begin("OgreOpcodeDebug/AABBs", Ogre::RenderOperation::OT_LINE_LIST);
		}
		
		void OgreOpcodeDebugger::endAABBs()
		{
			mAABBsDebugObject->end();
			mAABBsDebugNode->attachObject(mAABBsDebugObject); 
		}

		void OgreOpcodeDebugger::clearAll()
		{
			clearRadii();
			clearContacts();
			clearShapes();
			clearBBs();
			clearAABBs();
		}

	} // namespace Details

} // namespace OgreOpcode
