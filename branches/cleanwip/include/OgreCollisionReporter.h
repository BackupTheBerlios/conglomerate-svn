///////////////////////////////////////////////////////////////////////////////
///  @file OgreCollisionReporter.h
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
#ifndef __OgreCollisionReporter_h__
#define __OgreCollisionReporter_h__

#include <Ogre.h>
#include "OgreOpcodeExports.h"

namespace OgreOpcode
{
   class CollisionObject;
   
   /// Describes a contact between 2 CollisionObject%s.
   class _OgreOpcode_Export CollisionPair
   {
   public:
      CollisionObject *co1; ///< the first object involved in the collision
      CollisionObject *co2; ///< the second object involved in the collision
      Real tstamp; ///< the timestamp at which the collision occured
      Vector3 contact; ///< the point of contact 
      Real distance; ///< distance to contact
      Vector3 co1_normal; ///< co1's collision plane normal
      Vector3 co2_normal; ///< co2's collision plane normal
      Vector3 ce1_v0, ce1_v1, ce1_v2; ///< co1's tricoords
      Vector3 ce2_v0, ce2_v1, ce2_v2; ///< co2's tricoords 
   };

   /// Collect and manage CollisionPair%s.
   /// Manages a set of collision pairs. Makes sure that each collision
   /// between 2 objects is only tested/reported once, to help 
   /// the CollisionContext avoid redundant checks.
   class _OgreOpcode_Export CollisionReporter
   {
      static const int max_reports_per_object = 256;
      //enum { MAX_REPORTS_PER_OBJECT = 256 }; // enum hack, but better than a #define ;-)

      typedef std::map<int,CollisionPair*> CollPairs;
      CollPairs coll_pairs;
      CollisionPair *report_array[max_reports_per_object];

   private:

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
   public:
      CollisionReporter(void)
      {}
      
      /// initialize nKeyArray for new collision frame
      void BeginFrame(void)
      {
         // TODO I should delete the CollisionPair pointers!
         coll_pairs.clear();
         memset(report_array,0,sizeof(report_array));
      };

      /// check if a collision has already been reported
      bool CollissionExists(int id1, int id2)
      {
         // generate the merged 32 bit id, and query key array
         // for the collision
         int key;
         get_merged_id(id1,id2,key);
         CollPairs::iterator i = coll_pairs.find(key);
         if (i != coll_pairs.end())
         {
            return true;
         } else
            return false;
      };

      /// add a new collision 
      void AddCollission(CollisionPair& cr, int id1, int id2)
      {
         // generate the merged 32 bit id and add collision report
         int key;
         get_merged_id(id1,id2,key);
         CollPairs::const_iterator i = coll_pairs.find(key);
         if (i == coll_pairs.end())
         {
            coll_pairs[key] = &cr;
         }
      };

      /// end a collision frame
      void EndFrame(void) { };

      /// get overall number of collisions recorded
      int GetNumCollissions(void)
      {
         return coll_pairs.size();
      };

      /// report collisions for a specific object.
      /// returns number of collisions and pointer to an array of collision report
      /// pointers into the nKeyArray.
      int GetCollissions(CollisionObject *co, CollisionPair **& cr_ptr)
      {
         // fill report array with all collisions which this
         // object is involved in.
         int num_reports = 0;

         CollPairs::iterator i, iend;
         iend = coll_pairs.end();
         for (i = coll_pairs.begin(); i != iend; ++i)
         {
            if ((i->second->co1 == co) || (i->second->co2 == co)) 
            {
               report_array[num_reports++] = i->second;
            }
         }
         cr_ptr = report_array;
         return num_reports;
      }

      /// get all recorded collisions.
      int GetAllCollissions(CollisionPair **& cr_ptr) 
      {
         int num_reports = 0;
         CollPairs::iterator i, iend;
         iend = coll_pairs.end();
         for (i = coll_pairs.begin(); i != iend; ++i)
         {
            report_array[num_reports++] = i->second;
         }
         cr_ptr = report_array;
         return coll_pairs.size();
      }

      /// Get the collision closest to given point.
      /// @param  v       [in] origin coordinate
      /// @param  crPtr   [out] pointer to collide report pointer array
      /// @return         number of entries in collide report pointer array (0 or 1)
      int GetClosestCollission(const Vector3& v, CollisionPair **& crPtr)
      {
         if (coll_pairs.empty())
         {
            crPtr = 0;
            return 0;
         }

         Vector3 distVec;
         CollisionPair* minPtr = coll_pairs.begin()->second;
         float minDist = Vector3(minPtr->contact - v).length();

         CollPairs::iterator i, iend;
         iend = coll_pairs.end();
         for (i = coll_pairs.begin(); i != iend; ++i)
         {
            distVec = i->second->contact - v;
            Real dist = distVec.length();
            if (dist < minDist)
            {
               minDist = dist;
               minPtr  = i->second;
            }
         }
         report_array[0] = minPtr;
         crPtr = report_array;
         return 1;
      }
   };
}

#endif // __OgreCollisionReporter_h__