///////////////////////////////////////////////////////////////////////////////
///  @file OgreCollisionManager.cpp
///  @brief <TODO: insert file description here>
///
///  @author jacmoe @date 28-05-2005
///  
///  This file is part of OgreOpcode.
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
#include "OgreCollisionManager.h"
#include "OgreCollisionReporter.h"

template<> OgreOpcode::CollisionManager* Ogre::Singleton<OgreOpcode::CollisionManager>::ms_Singleton = 0;

namespace OgreOpcode
{

   CollisionManager& CollisionManager::getSingleton(void)
   {
      return Singleton<CollisionManager>::getSingleton();
   }

   CollisionManager::CollisionManager(SceneManager* sMgr)
   {
      mSceneMgr = sMgr;
      unique_id             = 0;
      default_context       = NULL;

      // setup the tree collider
      opcTreeCollider.SetFirstContact(false);       // report all contacts
      opcTreeCollider.SetFullBoxBoxTest(false);     // use coarse BV-BV tests
      opcTreeCollider.SetFullPrimBoxTest(false);    // use coarse primitive-BV tests
      opcTreeCollider.SetTemporalCoherence(false);  // don't use temporal coherence

      // setup the ray collider
      opcRayCollider.SetFirstContact(false);                // report all contacts
      opcRayCollider.SetTemporalCoherence(false);           // no temporal coherence
      opcRayCollider.SetClosestHit(false);                  // all hits
      opcRayCollider.SetCulling(true);                      // with backface culling
      opcRayCollider.SetMaxDist(100000.0f);                 // max dist 100 km
      opcRayCollider.SetDestination(&(opcFaceCache)); // detected hits go here

      // setup the sphere collider
      opcSphereCollider.SetFirstContact(false);             // report all contacts
      opcSphereCollider.SetTemporalCoherence(false);        // no temporal coherence
   }

   CollisionManager::~CollisionManager()
   {
      collclass_list.clear();
      // TODO Should I delete the pointers?
      shape_list.clear();
      colltype_table.clear();
   }

   CollisionContext *CollisionManager::NewContext(const String& contextName)
   {
      ContextIterator i = context_list.find(contextName);
      if (i != context_list.end())
      {
         // Warning! Context already exsists. Return the exsisting one.
         return i->second;
      }
      CollisionContext *cc = new CollisionContext();
      context_list.insert(ContextList::value_type(contextName,cc));
      return cc;
   }

   /// Create a new, possibly shared shape object.
   CollisionShape *CollisionManager::NewShape(const String& id)
   {
      String new_id = getResourceID(id);

      ShapeIterator i = shape_list.find(new_id);
      if (i != shape_list.end())
      {
         // Warning! Shape already exsists. Return the exsisting one.
         return i->second;
      }
      CollisionShape* cs = new CollisionShape(new_id);
      shape_list.insert(ShapeList::value_type(new_id,cs));
      return cs;
   }

   void CollisionManager::ReleaseContext(CollisionContext *cc)
   {
      ContextIterator i, iend;
      iend = context_list.end();
      for (i = context_list.begin(); i != iend; ++i)
      {
         if (i->second == cc)
         {
            context_list.erase(i->first);
            break;
         }
      }
   }

   void CollisionManager::ReleaseShape(CollisionShape *cs)
   {
      ShapeIterator i, iend;
      iend = shape_list.end();
      for (i = shape_list.begin(); i != iend; ++i)
      {
         if (i->second == cs)
         {
            shape_list.erase(i->first);
            break;
         }
      }
   }

   SceneManager *CollisionManager::getSceneManager(void)
   {
      return mSceneMgr;
   }
   
   CollisionContext *CollisionManager::GetContext(const String& name)
   {
      ContextIterator i = context_list.find(name);

      if (i == context_list.end())
      {
         // Error handling!
         // Just return the default context (for now)
         if(default_context)
            return default_context;
      }
      return i->second;
   }

   CollisionContext *CollisionManager::GetDefaultContext(void)
   {
      if (!default_context)
      {
         default_context = NewContext("default");
      }
      return default_context;
   }

   /// Get a resource id string from a path name, or create a unique
   /// resource id string if no name is given.
   String CollisionManager::getResourceID(const String& name)
   {
      char buf[512];
      if (name == "")
         sprintf(buf,"res%d",(int)unique_id++);
      else
      {
         // cut name to 32 characters and convert illegal chars
         // to underscores
         char c;
         char *str;
         int len = strlen(name.c_str())+1;
         int off = len - sizeof(buf);
         if (off < 0) off = 0;
         len -= off;
         strcpy(buf,&(name[off]));
         str = buf;
         while ((c = *str))
         {
            if ((c=='.')||(c=='/')||(c=='\\')||(c==':'))
               *str='_';
            str++;
         }
      }
      return String(buf);
   }

   void CollisionManager::AddCollClass(const String& cl_name)
   {
      CollClassIterator i = collclass_list.find(cl_name);
      if (i != collclass_list.end())
      {
         // Warning! Collision Class already exsists.
         return;
      }
      collclass_list.insert(CollClassList::value_type(cl_name,collclass_list.size()+1));
   }

   /// Important: Collision types MUST be bidirectional, if one object
   /// checks collision with another object, the collision type must
   /// be identical as if the check would be in the other direction.
   /// Due to the implementation of the top-level-collision check,
   /// one of the 2 checks may return false, although a collision may
   /// take place!
   void CollisionManager::AddCollType(const String& cl1, const String& cl2, CollisionType collType)
   {
      // Retrieve the first collision class
      CollClassIterator i = collclass_list.find(cl1);
      if (i == collclass_list.end())
      {
         // Warning! Collision Class not found.
         return;
      }
      int cc1 = i->second;
      
      // Retrieve the second collision class
      i = collclass_list.find(cl2);
      if (i == collclass_list.end())
      {
         // Warning! Collision Class not found.
         return;
      }
      int cc2 = i->second;

      // This key shouldn't exsist, but check anyway.
      int key;
      get_merged_id(cc1,cc2,key);
      CollTypeIterator h = colltype_table.find(key);
      if (h == colltype_table.end())
      {
         colltype_table.insert(CollTypeList::value_type(key,collType));
      }
      return;
   }

   CollisionClass CollisionManager::QueryCollClass(const String& cc)
   {
      CollClassIterator i = collclass_list.find(cc);

      if (i == collclass_list.end())
      {
         // Raise an exception here
      }

      return i->second;
   }

   CollisionType CollisionManager::QueryCollType(const String& s_cc1, const String& s_cc2)
   {
      CollClassIterator i = collclass_list.find(s_cc1);
      if (i == collclass_list.end())
      {
         // Error! Collision Class not found.
      }
      CollisionClass class1 = i->second;
      i = collclass_list.find(s_cc2);
      if (i == collclass_list.end())
      {
         // Error! Collision Class not found.
      }
      CollisionClass class2 = i->second;

      return QueryCollType(class1,class2);
   }

};