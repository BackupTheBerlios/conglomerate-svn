///////////////////////////////////////////////////////////////////////////////
///  @file OgreCollisionShape.h
///  @brief <TODO: insert file description here>
///
///  @author jacmoe @date 29-05-2005
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
#ifndef __OgreCollisionShape_h__
# define __OgreCollisionShape_h__

#include "OgreOpcodeExports.h"
# include <Ogre.h>

#include "OgreHash.h"
#include "OgreCollisionTypes.h"
#include "OgreOpcodeDebugObject.h"
#include "Opcode.h"

using namespace OgreOpcode::Details;

namespace OgreOpcode
{
   class CollisionPair;

   /// Describes shapes for collision system.
   /// Holds a triangle list describing a collision shape.
   /// One CollisionShape object may be shared between several
   /// CollisionObject%s. 2 CollisionShape objects may also
   /// be queried directly whether they intersect.
   ///
   /// CollisionShape objects are also able to load themselves
   /// from a mesh file.
   class _OgreOpcode_Export CollisionShape : public nHashNode
   {
   public:
      /// Constructs a CollisionShape
      CollisionShape(const char* name);

      virtual ~CollisionShape();

      /// increment refcount by one
      int AddRef();
      /// decrement refcount by one
      int RemRef();
      /// get current refcount
      int GetRef();
      /// has object been initialized?
      bool IsInitialized();
      /// get radius of collide mesh
      Real GetRadius();
      /// load collide geometry from mesh
      virtual bool Load(Entity* ent);
      /// perform collision with other CollisionShape
      virtual bool Collide(CollisionType collType, Matrix4& ownMatrix, CollisionShape* otherShape, Matrix4& otherMatrix, CollisionPair& collPair);
      /// perform collision with line
      virtual bool LineCheck(CollisionType collType, const Matrix4& ownMatrix, const Ray& line, const Real dist, CollisionPair& collPair);
      /// perform a sphere check
      virtual bool SphereCheck(CollisionType collType, const Matrix4& ownMatrix, const Sphere& ball, CollisionPair& collPair);
      /// visualize the collide shape
      virtual void Visualize();
      /// toggle debug rendering.
      void setDebug(bool debug);
      /// return entity
      Entity* getEntity();

      Vector3 getSize();
      Vector3 getCenter();
      Real getHeight();

   private:
      void countIndicesAndVertices(Entity * entity, size_t & index_count, size_t & vertex_count);
      void convertMeshData(Entity * entity, float * vertexData, size_t vertex_count, int * faceData, size_t index_count);
      /// get tri coords from tri index
      void GetTriCoords(int index, Vector3& v0, Vector3& v1, Vector3& v2);
      /// visualize the AABBTree of the opcode model
      void VisualizeAABBCollisionNode(const Opcode::AABBCollisionNode* node);

      void calculateSize();

      Vector3 mSize;
      
      Opcode::BVTCache*         opcTreeCache;
      Opcode::CollisionFaces*   opcFaceCache;

      Opcode::MeshInterface opcMeshAccess;
      Opcode::Model opcModel;
      int numVertices;
      int numFaces;
      float* vertexData;
      int*   faceData;
      Entity* mEntity;

      bool isInitialized;
      int refCount;
      float radius;

      /// prevent default construction
      CollisionShape();

      DebugObject* _debug_obj;
      void createDebugObject();
      void destroyDebugObject();
   };

   inline
   int
   CollisionShape::AddRef()
   {
      return ++refCount;
   }

   inline
   int
   CollisionShape::RemRef()
   {
      assert(refCount > 0);
      return --refCount;
   }

   inline
   int
   CollisionShape::GetRef()
   {
      return refCount;
   }

   inline
   bool
   CollisionShape::IsInitialized()
   {
      return isInitialized;
   }

   inline
   Real
   CollisionShape::GetRadius()
   {
      return radius;
   }

   inline Vector3 CollisionShape::getSize()
   {
      return mSize;
   }

   inline Vector3 CollisionShape::getCenter()
   {
      return Vector3(0, 0, getHeight() / 2.0);
   }

   inline Real CollisionShape::getHeight()
   {
      return getSize().y;
   }

   /// Extract triangle coordinates from triangle index.
   inline
      void
      CollisionShape::GetTriCoords(int index, Vector3& v0, Vector3& v1, Vector3& v2)
   {
      int* indexPtr = &(faceData[3 * index]);
      float* vp0 = &(vertexData[3 * indexPtr[0]]);
      float* vp1 = &(vertexData[3 * indexPtr[1]]);
      float* vp2 = &(vertexData[3 * indexPtr[2]]);
      v0 = Vector3(vp0[0], vp0[1], vp0[2]);
      v1 = Vector3(vp1[0], vp1[1], vp1[2]);
      v2 = Vector3(vp2[0], vp2[1], vp2[2]);
   }
   
}; // namespace OgreOpcode

#endif // __OgreCollisionEntity_h__
