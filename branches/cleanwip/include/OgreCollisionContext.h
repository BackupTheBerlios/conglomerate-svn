///////////////////////////////////////////////////////////////////////////////
///  @file OgreCollisionContext.h
///  @brief Contains the definition of the CollisionContext class.
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
#ifndef __OgreCollisionContext_h__
#define __OgreCollisionContext_h__

#include <Ogre.h>
#include "OgreCollisionReporter.h"
#include "OgreCollisionTypes.h"

namespace OgreOpcode
{
   typedef int CollisionClass;
   class CollisionObject;
   /// Defines a collision space.
   /// A CollisionContext creates a collision context, defined by
   /// a collection of CollisionObject%s which can collide with 
   /// each other. CollisionObject%s can be added and removed
   /// from the context at any time.
   class _OgreOpcode_Export CollisionContext
   {
   public:
      /// constructor
      CollisionContext();
      /// destructor
      virtual ~CollisionContext();
      /// create a collide object
      virtual CollisionObject *NewObject(void);
      /// release a collide object
      virtual void ReleaseObject(CollisionObject *collObj);
      /// add collide object to context
      virtual void AddObject(CollisionObject *collObj);
      /// remove collide object from context
      virtual void RemoveObject(CollisionObject *collObj);
      /// compute contacts between collision objects in context
      virtual int Collide(void);
      /// debug visualization of the collide context
      virtual void Visualize(bool makeVisible);
      /// get the collide reports for the collisions computed inside Collide()
      virtual int GetCollissions(CollisionObject *collObj, CollisionPair **&cpPtr);
      /// do a "moving sphere" check against collide object radii in the context
      virtual int MovingSphereCheck(const Vector3& p0, const Vector3& v0, Real radius, CollisionClass collClass, CollisionPair **& cpPtr);
      /// do a line-model check
      virtual int RayCheck(const Ray ray, const Real dist, CollisionType collType, CollisionClass collClass, CollisionPair**& cpPtr);
      /// do a sphere-model check
      virtual int SphereCheck(const Sphere& sphere, CollisionType collType, CollisionClass collClass, CollisionPair**& cpPtr);
      /// reset position and timestamp of all objects
      virtual void Reset();

   private:
      friend class CollisionObject;

      static const int maxnum_collisions = 4096;
      //enum { MAXNUM_COLLISSIONS = 4096 };

      CollisionReporter collideReportHandler;     ///< collide reports for Collide()
      CollisionReporter checkReportHandler;       ///< collide reports for Check() functions

   protected:
      std::list<CollisionObject*> owned_list;       ///< list of CollisionObject%s created by this context
      std::list<CollisionObject*> attached_list;    ///< the list of objects currently attached to the context
      typedef std::list<CollisionObject*>::const_iterator attached_list_iterator;
      int unique_id;
   };

   inline CollisionContext::CollisionContext() :
      unique_id(0)
   {
      // empty
   }

}

#endif // __OgreCollisionContext_h__