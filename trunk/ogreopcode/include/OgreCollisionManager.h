///////////////////////////////////////////////////////////////////////////////
///  @file OgreCollisionManager.h
///  @brief <TODO: insert file description here>
///
///  @author The OgreOpcode Team @date 29-05-2005
///  
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
#ifndef __OgreCollisionManager_h__
#define __OgreCollisionManager_h__

#include "OgreOpcodeExports.h"
#include <Ogre.h>
#include "OgreCollisionTypes.h"
#include "OgreNodes.h"
#include "OgreCollisionContext.h"
#include "OgreCollisionShape.h"
#include "Opcode.h"

#include <list>
#include <map>

using namespace OgreOpcode::Details;

/// Main %OgreOpcode namespace
namespace OgreOpcode
{
	typedef int CollisionClass;

	/// Collision manager.
	/// The CollisionManager object serves as factory object of the
	/// different classes of the collision system, namely
	/// CollisionContext and CollisionShape. A CollisionContext
	/// serves as factory for CollisionObject%s.
	class _OgreOpcode_Export CollisionManager : public Singleton<CollisionManager>
	{
		friend class CollisionShape;
	public:
		CollisionManager(SceneManager *);
		virtual ~CollisionManager();

		static CollisionManager& getSingleton(void);

		CollisionContext *newContext(const String&);
		CollisionShape   *newShape(const String&);
		void releaseContext(CollisionContext *);
		void releaseShape(CollisionShape *);

		CollisionContext *getDefaultContext(void);
		CollisionContext *getContext(const String& name);
		SceneManager *getSceneManager(void);

		// define collision classes and collision check relationships
		void beginCollClasses(void);
		void addCollClass(const String &);
		void endCollClasses(void);

		void beginCollTypes(void);
		void addCollType(const String&, const String&, CollisionType);
		void endCollTypes(void);

		CollisionClass queryCollClass(const String&);
		CollisionType queryCollType(const String&, const String&);

		CollisionType queryCollType(CollisionClass cc1, CollisionClass cc2)
		{

			// check for CollClass override cases
			if ((cc1 == COLLTYPE_ALWAYS_IGNORE) || (cc2 == COLLTYPE_ALWAYS_IGNORE))
			{
				return COLLTYPE_IGNORE;
			}
			else if ((cc1 == COLLTYPE_ALWAYS_QUICK) || (cc2 == COLLTYPE_ALWAYS_QUICK))
			{
				return COLLTYPE_QUICK;
			}
			else if ((cc1 == COLLTYPE_ALWAYS_CONTACT) || (cc2 == COLLTYPE_ALWAYS_CONTACT))
			{
				return COLLTYPE_CONTACT;
			}
			else if ((cc1 == COLLTYPE_ALWAYS_EXACT) || (cc2 == COLLTYPE_ALWAYS_EXACT))
			{
				return COLLTYPE_EXACT;
			}
			//			assert(colltype_table);
			assert((cc1 >= 0) && (cc2 >= 0));
			assert(int(cc1) < num_coll_classes);
			assert(int(cc2) < num_coll_classes);

			int key;
			get_merged_id(cc1, cc2, key);
			return colltype_table[key];
		};
	protected:
		int unique_id;
		typedef std::map<String,CollisionContext*> ContextList;
		typedef ContextList::const_iterator ContextIterator;
		ContextList context_list;

		typedef HashMap<String,CollisionShape*> ShapeList;
		// TODO: Do I own these shapes? Or do I pass the responsibility on?
		ShapeList shape_list;
		typedef ShapeList::const_iterator ShapeIterator;

		typedef std::map<String, CollisionClass> CollClassList;
		CollClassList collclass_list;
		typedef CollClassList::const_iterator CollClassIterator;
		typedef std::map<int, CollisionType> CollTypeList;
		CollTypeList colltype_table;
		typedef CollTypeList::const_iterator CollTypeIterator;

		CollisionContext *default_context;

		bool in_begin_collclasses;
		bool in_begin_colltypes;
		int num_coll_classes;

		SceneManager *mSceneMgr;
		String getResourceID(const String&);
	private:
		Opcode::AABBTreeCollider opcTreeCollider;
		Opcode::RayCollider      opcRayCollider;
		Opcode::SphereCollider   opcSphereCollider;
		Opcode::PlanesCollider   opcPlanesCollider;
		Opcode::LSSCollider      opcLSSCollider;
		Opcode::BVTCache         opcTreeCache;
		Opcode::CollisionFaces   opcFaceCache;
		Opcode::SphereCache      opcSphereCache;
		Opcode::PlanesCache      opcPlanesCache;
		Opcode::LSSCache         opcLSSCache;

		// Merge the 2 object id's into 1 32 bit id,
		// order them, so that any combination of 2 id's
		// results in the same merged id. Return true
		// a swap happend (because other attributes
		// may have to be swapped as well).
		bool get_merged_id(int id1, int id2, int& mrg)
		{
			if (id1 > id2)
			{
				mrg = ((id2 & 0xffff)<<16) | (id1 & 0xffff);
				return true;
			} else
			{
				mrg = ((id1 & 0xffff)<<16) | (id2 & 0xffff);
				return false;
			}
		};
	};

}; // namespace OgreOpcode

#endif // __OgreCollisionManager_h__
