///////////////////////////////////////////////////////////////////////////////
///  @file OgreCollisionObject.h
///  @brief <TODO: insert file description here>
///
///  @author jacmoe @date 29-05-2005
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
#ifndef __OgreCollisionObject_h__
#define __OgreCollisionObject_h__

#include <Ogre.h>
#include "OgreCollisionManager.h"
#include "OgreCollisionContext.h"

namespace OgreOpcode
{
   class CollisionContext;
   class CollisionPair;

   /// Collision system object.
   /// CollisionObject is an actual collision system object which can
   /// be positioned and oriented in space. It points to an
   /// CollisionShape which describes the actual shape of the
   /// object.
   /// CollisionObject%s are kept in sorted list (one for each dimension)
   /// by the CollisionContext they belong to.
   class _OgreOpcode_Export CollisionObject
   {
      friend class CollisionContext;

   protected:
      int id;                     ///< a unique 32 bit id for this object
      CollisionContext *context;    ///< the collide context this object is currently attached to

      Real radius;               ///< radius of the collision object (normally provided by shape)
      CollisionShape *shape;       ///< the triangle exact collision shape (optional)
      CollisionClass coll_class;      ///< the application defined collision type

      Matrix4 old_matrix;        ///< the previous position/orientation of the object
      Matrix4 new_matrix;        ///< the new position/orientation of the object
      Real old_tstamp;          ///< the timestamp for 'old_transform'
      Real new_tstamp;          ///< the timestamp for 'new_transform'

      bool is_attached;           ///< currently attached to a context

      int num_colls;              ///< number of collisions this object is involved in

   public:
      CollisionObject()
         : context(NULL),
         radius(0.0f),
         shape(NULL),
         coll_class(0),
         old_tstamp(0.0),
         new_tstamp(0.0),
         is_attached(false),
         num_colls(0)
      {
      };

      /// <TODO: insert function description here>
      /// @param [in, out]  c Ogre::CollisionContext *    <TODO: insert parameter description here>
      /// @return void <TODO: insert return value description here>
      void SetContext(CollisionContext *c)
      {
         // c may be NULL!!!
         context = c;
      };

      /// <TODO: insert function description here>
      /// @return Ogre::CollisionContext * <TODO: insert return value description here>
      CollisionContext *GetContext(void)
      {
         return context;
      };

      /// <TODO: insert function description here>
      /// @param [in]       i int     <TODO: insert parameter description here>
      /// @return void <TODO: insert return value description here>
      void SetId(int i)
      {
         id = i;
      };
      
      /// <TODO: insert function description here>
      /// @return int <TODO: insert return value description here>
      int GetId(void)
      {
         return id;
      };

      /// <TODO: insert function description here>
      /// @param [in]       b bool     <TODO: insert parameter description here>
      /// @return void <TODO: insert return value description here>
      void SetAttached(bool b)
      {
         is_attached = b;
      };
      
      /// <TODO: insert function description here>
      /// @return bool <TODO: insert return value description here>
      bool IsAttached(void)
      {
         return is_attached;
      };

      /// <TODO: insert function description here>
      /// @param [in]       f float     <TODO: insert parameter description here>
      /// @return void <TODO: insert return value description here>
      void SetRadius(float f)
      {
         radius = f;
      };
      
      /// <TODO: insert function description here>
      /// @return float <TODO: insert return value description here>
      float GetRadius(void)
      {
         return radius;
      };

      /// <TODO: insert function description here>
      /// @param [in, out]  s CollisionShape *    <TODO: insert parameter description here>
      /// @return void <TODO: insert return value description here>
      void SetShape(CollisionShape *s)
      {
         shape = s;
         if (s)
         {
            SetRadius(s->GetRadius());
         }
         else
         {
            SetRadius(0.0f);
         }
      };
      
      /// <TODO: insert function description here>
      /// @return CollisionShape * <TODO: insert return value description here>
      CollisionShape *GetShape(void)
      {
         return shape;
      };

      /// <TODO: insert function description here>
      /// @param [in]       cc CollisionClass     <TODO: insert parameter description here>
      /// @return void <TODO: insert return value description here>
      void SetCollClass(CollisionClass cc)
      {
         coll_class = cc;
      };
      
      /// <TODO: insert function description here>
      /// @return CollisionClass <TODO: insert return value description here>
      CollisionClass GetCollClass(void)
      {
         return coll_class;
      };

      /// <TODO: insert function description here>
      /// @return const Matrix4 & <TODO: insert return value description here>
      const Matrix4& GetTransform(void)
      {
         GetShape()->getEntity()->getSubEntity(0)->getWorldTransforms(&new_matrix);
         return new_matrix;
      };
      
      /// <TODO: insert function description here>
      /// @return const Matrix4 & <TODO: insert return value description here>
      const Matrix4& GetPrevTransform(void)
      {
         return old_matrix;
      };

      /// <TODO: insert function description here>
      /// @return Real <TODO: insert return value description here>
      Real GetTimeStamp(void)
      {
         return new_tstamp;
      };
      
      /// <TODO: insert function description here>
      /// @return Real <TODO: insert return value description here>
      Real GetPrevTimeStamp(void)
      {
         return old_tstamp;
      };

      /// <TODO: insert function description here>
      /// @return void <TODO: insert return value description here>
      void ClearCollissions(void)
      {
         num_colls = 0;
      };
      
      /// <TODO: insert function description here>
      /// @return int <TODO: insert return value description here>
      int GetNumCollissions(void)
      {
         return num_colls;
      };

      /// Transform the object to its new position/orientation, update the dimensional nodes and the bounding box.
      /// @param [in]       t Real     timestamp
      /// @param [in]       m const Matrix4 &    world transform
      void Transform(Real t, Matrix4& m)
      {
         assert(is_attached);

         GetShape()->getEntity()->getSubEntity(0)->getWorldTransforms(&m);
         // if old_matrix and old_tstamp are not yet valid,
         // they will be initialized with the current
         // values, to prevent "startup popping"
         if (old_tstamp == 0.0)
         {
            old_matrix = m;
            old_tstamp = t;
         } else if (old_tstamp == t)
         {
            old_matrix = m;
            old_tstamp = t;
         } else
         {
            old_matrix = new_matrix;
            old_tstamp = new_tstamp;
         }
         new_matrix = m;
         new_tstamp = t;

         // update the bounding-box

      };

      /// Check whether 2 moving collide objects have contact.
      bool Contact(CollisionObject *other,     // the other object
         CollisionType ct,
         CollisionPair& cr)
      {
         Vector3 p0(old_matrix[0][3], old_matrix[1][3], old_matrix[2][3]);
         Vector3 p1(other->old_matrix[0][3], other->old_matrix[1][3], other->old_matrix[2][3]);
         Vector3 v0(Vector3(new_matrix[0][3], new_matrix[1][3], new_matrix[2][3])  - p0);
         Vector3 v1(Vector3(other->new_matrix[0][3], other->new_matrix[1][3], other->new_matrix[2][3]) - p1);

         bool has_contact = false;
         switch (ct)
         {
         case COLLTYPE_QUICK:
            {
            // do a contact check between 'moving spheres'

//               if (s0.intersect_sweep(v0,s1,v1,u0,u1))
            }
            break;

         case COLLTYPE_EXACT:
         case COLLTYPE_CONTACT:
            {
               // If distance travelled is more then 1/8 then each of the object's
               // radii, then we do several tests along the line
               // of movements.
            }
            break;

         default:
            break;
         }
         return has_contact;
      };

      /// For each overlapping object in all 3 dimensions,
      /// which doesn't fall into the ignore_types category,
      /// do a collision check, and if the check is positive,
      /// record collision by doing an AddCollission().
      void Collide(void)
      {
      };

      /// Return collision reports for all collisions this object is involved in.
      int GetCollissions(CollisionPair **&crp)
      {
         assert(context);
         assert(is_attached);
         return context->collideReportHandler.GetCollissions(this,crp);
         return 0;
      };

   };
};

#endif // __OGRECOLLIDER_H__
