///////////////////////////////////////////////////////////////////////////////
///  @file OgreCollisionContext.cpp
///  @brief <TODO: insert file description here>
///
///  @author jacmoe @date 28-05-2005
///  
///////////////////////////////////////////////////////////////////////////////
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
#include "OgreCollisionContext.h"
#include "OgreCollisionObject.h"
#include "OgreCollisionManager.h"

namespace OgreOpcode
{
   // release all owned collide objects
   CollisionContext::~CollisionContext()
   {
      
      // remove collision objects
      while (!owned_list.empty())
      {
         ReleaseObject(*(owned_list.begin()));
      }
   }

   // Construct a new collide object.
   CollisionObject *CollisionContext::NewObject(void)
   {
      CollisionObject *co = new CollisionObject();
      co->SetId(unique_id++);
      co->SetContext(this);
      owned_list.push_back(co);
      return co;
   }

   // Kill an owned collide object.
   void CollisionContext::ReleaseObject(CollisionObject *collObj)
   {
      if (collObj != NULL)
      {
         if(collObj->IsAttached())
            attached_list.remove(collObj);
         owned_list.remove(collObj);
         collObj->SetAttached(false);
         //collObj->xmin_cnode.Remove();
         //collObj->xmax_cnode.Remove();
         collObj->SetContext(NULL);
         delete collObj;
      }
   }
   
   void CollisionContext::AddObject(CollisionObject *collObj)
   {
      collObj->SetAttached(true);
      attached_list.push_back(collObj);

      // add minx/maxx nodes to x-dimensional sorted list
//      collObj->xmin_cnode.AddToList(this->xdim_list);
//      collObj->xmax_cnode.AddToList(this->xdim_list);
}

   void CollisionContext::RemoveObject(CollisionObject *collObj)
   {
      if (collObj != NULL)
      {
         collObj->SetAttached(false);
         attached_list.remove(collObj);
         //         collObj->context_node.Remove();
         //         collObj->xmin_cnode.Remove();
         //         collObj->xmax_cnode.Remove();
      }
   }

   /// Call collide on each object in the context.
   /// After this, each object's collision array holds all collisions
   /// this object was involved with.
   int CollisionContext::Collide(void)
   {
      // first, clear the collision counters in all collide objects
      for (attached_list_iterator i = attached_list.begin(); i != attached_list.end(); ++i)
      {
         (*i)->ClearCollissions();
      }

      // check the collision status for each object
      collideReportHandler.BeginFrame();

      for (attached_list_iterator i = attached_list.begin(); i != attached_list.end(); ++i)
      {
         (*i)->Collide();
      }
      
      collideReportHandler.EndFrame();

      return collideReportHandler.GetNumCollissions();
   }

   /// Get all collisions an object is involved in.
   /// Returns pointer to an internal collision array and
   /// the number of collisions.
   int CollisionContext::GetCollissions(CollisionObject *collObj, CollisionPair **&cpPtr)
   {
      if (collObj->GetNumCollissions() > 0)
      {
         return collideReportHandler.GetCollissions(collObj,cpPtr);
      } else
      {
         cpPtr = NULL;
         return 0;
      }
   }

   /// Visualize all objects in the context.
   void CollisionContext::Visualize(bool makeVisible)
   {
   }

   /// Do an instant check of a moving sphere in the collision volume.
   /// Fills out the provided collide report array and
   /// returns number of detected collisions.
   /// @param p0     [in] starting position
   /// @param v0     [in] vector to ending position
   /// @param radius [in] radius
   /// @param collClass [in] collision class for collision type query
   /// @param cr_ptr [out] pointer to array of pointers to CollisionPair's
   /// @return       number of detected contacts (1 per collide object)
   int CollisionContext::MovingSphereCheck(const Vector3& p0, const Vector3& v0, Real radius, CollisionClass collClass, CollisionPair **& cpPtr)
   {
      return 0;
   }

   /// Test a ray against the collide objects in the collide context.
   /// The collType will be interpreted as follows:
   /// - COLLTYPE_IGNORE:        illegal (makes no sense)
   /// - COLLTYPE_QUICK:         occlusion check only
   /// - COLLTYPE_CONTACT:       return closest contact only
   /// - COLLTYPE_EXACT:         return all contacts (unsorted)
   /// @param  line        [in]  the ray to test in global space
   /// @param  collType    [in]  the collision type
   /// @param  collClass   [in]  optional coll class (COLLCLASS_ALWAYS_* if no coll class filtering wanted)
   /// @param  cpPtr       [out] will be filled with pointer to collide report pointers
   /// @return             number of detected contacts (1 per collide object)
   int CollisionContext::RayCheck(const Ogre::Ray ray, const Real dist, CollisionType collType, CollisionClass collClass, CollisionPair**& cpPtr)
   {
      return 0;
   }

   /// Test a sphere against the collide objects in the collide context.
   /// The collType will be interpreted as follows:
   /// - COLLTYPE_IGNORE:        illegal (makes no sense)
   /// - COLLTYPE_QUICK:         return all contacts, do sphere-sphere check
   /// - COLLTYPE_CONTACT:       return closest contact only, sphere-shape
   /// - COLLTYPE_EXACT:         return all contacts (unsorted), sphere-shape
   /// @param  theSphere      [in]  the sphere to test in global space
   /// @param  collType    [in]  the collission type
   /// @param  collClass   [in]  optional coll class (COLLCLASS_ALWAYS_* if no coll class filtering wanted)
   /// @param  cpPtr       [out] will be filled with pointer to collide report pointers
   /// @return             number of detected contacts (1 per collide object)
   int CollisionContext::SphereCheck(const Ogre::Sphere& sphere, CollisionType collType, CollisionClass collClass, CollisionPair**& cpPtr)
   {
      return 0;
   }


   /// Reset position and timestamp of all attached collide objects to 0.0.
   /// This is useful at the beginning of a level to prevent phantom collissions
   /// (when objects are repositioned to their starting point in the level).
   void CollisionContext::Reset()
   {
   }
}