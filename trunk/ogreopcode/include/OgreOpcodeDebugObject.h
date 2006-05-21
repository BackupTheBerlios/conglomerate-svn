///////////////////////////////////////////////////////////////////////////////
///  @file OgreOpcodeDebugObject.h
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
#ifndef __OgreOpcodeDebugObject_h__
#define __OgreOpcodeDebugObject_h__

#include "OgreOpcodeExports.h"
# include "Ogre.h"

namespace OgreOpcode
{
	namespace Details
	{
		class _OgreOpcode_Export OgreOpcodeDebugger : public Singleton<OgreOpcodeDebugger>
		{
		public:
			OgreOpcodeDebugger(SceneManager* sceneMgr);
			virtual ~OgreOpcodeDebugger() {};
			
			static OgreOpcodeDebugger& getSingleton(void);
			static OgreOpcodeDebugger* getSingletonPtr(void);

			void addRadiiLine(Real lx1, Real ly1, Real lz1, Real lx2, Real ly2, Real lz2);
			void addContactLine(Real lx1, Real ly1, Real lz1, Real lx2, Real ly2, Real lz2);
			void addBBLine(Real lx1, Real ly1, Real lz1, Real lx2, Real ly2, Real lz2);
			void addShapeLine(Real lx1, Real ly1, Real lz1, Real lx2, Real ly2, Real lz2);
			void addAABBLine(Real lx1, Real ly1, Real lz1, Real lx2, Real ly2, Real lz2);

			void clearAll();

			void clearRadii();
			void beginRadii();
			void endRadii();
			void clearContacts();
			void beginContacts();
			void endContacts();
			void clearBBs();
			void beginBBs();
			void endBBs();
			void clearShapes();
			void beginShapes();
			void endShapes();
			void clearAABBs();
			void beginAABBs();
			void endAABBs();

		private:

			SceneManager*	mSceneMgr;

			SceneNode*		mRadiiDebugNode;
			SceneNode*		mContactsDebugNode;
			SceneNode*		mBBsDebugNode;
			SceneNode*		mShapesDebugNode;
			SceneNode*		mAABBsDebugNode;

			ManualObject*	mRadiiDebugObject;
			ManualObject*	mContactsDebugObject;
			ManualObject*	mBBsDebugObject;
			ManualObject*	mShapesDebugObject;
			ManualObject*	mAABBsDebugObject;
		};

	} // namespace Details

} // namespace OgreOpcode

#endif // __OgreOpcodeDebugObject_h__
