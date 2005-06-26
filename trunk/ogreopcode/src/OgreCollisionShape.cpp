///////////////////////////////////////////////////////////////////////////////
///  @file OgreCollisionShape.cpp
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
#include "OgreOpcodeExports.h"
#include "OgreCollisionShape.h"
#include "OgreCollisionReporter.h"
#include "OgreCollisionManager.h"

namespace OgreOpcode
{
   CollisionShape::CollisionShape(const String& name)
      : isInitialized(false),
      radius(0.0f),
      numVertices(0),
      numFaces(0),
      vertexData(0),
      faceData(0)
   {
      // initialize pointers to global OPCODE objects
      opcTreeCache    = &(CollisionManager::getSingletonPtr()->opcTreeCache);
      opcFaceCache    = &(CollisionManager::getSingletonPtr()->opcFaceCache);
   }

   CollisionShape::~CollisionShape()
   {
      if (vertexData)
      {
         free(vertexData);
         vertexData = 0;
      }
      if (faceData)
      {
         free(faceData);
         faceData = 0;
      }
   }

   /// Begins a collision shape.
   ///  @param [in]       numVertices int     Number of vertices to reserve.
   ///  @param [in]       numTriangles int     Number of triangles to reserve.
   void CollisionShape::Begin(int numVertices, int numTriangles)
   {
      assert(!vertexData);
      assert(!faceData);

      numVertices = numVertices;
      numFaces    = numTriangles;
      vertexData  = (float*) malloc(numVertices * 3 * sizeof(float));
      faceData    = (int*)   malloc(numTriangles  * 3 * sizeof(int));

      assert(!isInitialized);
      radius = 0.0f;
   }

   /// Fills out a vertex in the collision mesh.
   ///  @param [in]       index int     Vertex index.
   ///  @param [in, out]  v Vector3 &    Vector.
   void CollisionShape::SetVertex(int index, Vector3& v)
   {
      assert(vertexData);
      assert((index >= 0) && (index < numVertices));

      float* ptr = vertexData + 3 * index;
      ptr[0] = v.x;
      ptr[1] = v.y;
      ptr[2] = v.z;
      float l = v.length();
      if (l > radius)
      {
         radius = l;
      }
   }

   /// Fills out a triangle in the collision mesh.
   /// @param [in]       index int     Index into the index array.
   /// @param [in]       v0Index int     First index into the vertex array.
   /// @param [in]       v1Index int     Second index into the vertex array.
   /// @param [in]       v2Index int     Third index into the vertex array.
   void CollisionShape::SetTriangle(int index, int v0Index, int v1Index, int v2Index)
   {
      assert(faceData);
      assert((index >= 0) && (index < numFaces));
      assert((v0Index >= 0) && (v0Index < numVertices));
      assert((v1Index >= 0) && (v1Index < numVertices));
      assert((v2Index >= 0) && (v2Index < numVertices));

      int* ptr = faceData + 3 * index;
      ptr[0] = v0Index;
      ptr[1] = v1Index;
      ptr[2] = v2Index;
   }

   /// Ends a collision shape.
   void CollisionShape::End()
   {
      assert((numVertices > 0) && (numFaces > 0));
      assert(faceData && vertexData);

      opcMeshAccess.SetNbTriangles(numFaces);
      opcMeshAccess.SetNbVertices(numVertices);
      // not using callbacks anymore in order to comply with ODE tri-collider
      //opcMeshAccess.SetCallback(nOpcodeShape::collCallback, this);
      opcMeshAccess.SetPointers((IndexedTriangle*)faceData, (Point*)vertexData);
      opcMeshAccess.SetStrides(sizeof(int) * 3, sizeof(float) * 3);

      // Build tree
      Opcode::BuildSettings Settings;
      Settings.mRules = Opcode::SPLIT_BEST_AXIS; // dunno what this means, stole it from ODE :)

      Opcode::OPCODECREATE opcc;
      opcc.mIMesh = &opcMeshAccess;
      opcc.mSettings.mRules = Opcode::SPLIT_SPLATTER_POINTS;   // split by splattering primitive centers (???)
      opcc.mSettings.mLimit = 1;              // build a complete tree, 1 primitive/node
      opcc.mNoLeaf    = false; // true;
      opcc.mQuantized = false; // true;
      opcModel.Build(opcc);

      assert(!isInitialized);
      isInitialized = true;
   }

   /*
   OPCODE uses a callback function to actually get triangle data for the
   collision test.
   */
   /*
   void
   nOpcodeShape::collCallback(udword triangleIndex, VertexPointers& triangle, void * userData)
   {
   nOpcodeShape* self = (nOpcodeShape*) userData;
   int *indexPtr = &(self->faceData[3 * triangleIndex]);
   triangle.Vertex[0] = (Point*) &(self->vertexData[3 * indexPtr[0]]);
   triangle.Vertex[1] = (Point*) &(self->vertexData[3 * indexPtr[1]]);
   triangle.Vertex[2] = (Point*) &(self->vertexData[3 * indexPtr[2]]);
   }
   */
   
   /// perform collision with other CollisionShape.
   /// @param collType CollisionType     Collision type.
   /// @param ownMatrix Matrix4          Own matrix.
   /// @param otherShape CollisionShape     Shape to test against.
   /// @param otherMatrix Matrix4     Other matrix.
   /// @param collPair CollisionPair     Collision report.
   /// @return bool return true on collision.
   bool CollisionShape::Collide(CollisionType collType, Matrix4& ownMatrix, CollisionShape* otherShape, Matrix4& otherMatrix, CollisionPair& collPair)
   {
      return false;
   }

   /// Check contact of line with shape.
   /// The collType is interpreted as follows:
   /// - COLLTYPE_IGNORE:        illegal (makes no sense)
   /// - COLLTYPE_QUICK:         occlusion check only
   /// - COLLTYPE_CONTACT:       return closest contact
   /// - COLLTYPE_EXACT:         same as closest contact
   /// @param  collType        see above
   /// @param  ownMatrix       position/orientation of this shape
   /// @param  line            line definition in world space
   /// @param  collPair      will be filled with result
   /// @return                 true if line intersects shape
   bool CollisionShape::RayCheck(CollisionType collType,
      const Matrix4& ownMatrix,
      const Ogre::Ray& ray,
      const Real dist,
      CollisionPair& collPair)
   {
      return false;
   }

   /// Check contact of a sphere with shape.
   /// The collType is interpreted as follows:
   /// - COLLTYPE_IGNORE:        illegal (makes no sense)
   /// - COLLTYPE_QUICK:         first contact check only
   /// - COLLTYPE_CONTACT:       return closest contact
   /// - COLLTYPE_EXACT:         same as closest contact
   /// Currently, sphere checks always work in first constact mode (COLLTYPE_QUICK).
   /// @param  collType        see above
   /// @param  ownMatrix       position/orientation of this shape
   /// @param  ball          sphere definition in world space
   /// @param  collPair      will be filled with result
   /// @return                 true if line intersects shape
   bool CollisionShape::SphereCheck(CollisionType collType,
      const Matrix4& ownMatrix,
      const Ogre::Sphere& ball,
      CollisionPair& collPair)
   {
      return false;
   }

   /// <TODO: insert function description here>
   /// @param [in]       mesh const Mesh *const     <TODO: insert parameter description here>
   /// @param [in, out]  vertex_count size_t &    <TODO: insert parameter description here>
   /// @param [in, out]  vertices Vector3 *&    <TODO: insert parameter description here>
   /// @param [in, out]  index_count size_t &    <TODO: insert parameter description here>
   /// @param [in, out]  indices unsigned long *&    <TODO: insert parameter description here>
   /// @param [in]       position const Vector3 & [=Vector3::ZERO]    <TODO: insert parameter description here>
   /// @param [in]       orient const Quaternion & [=Quaternion::IDENTITY]    <TODO: insert parameter description here>
   /// @param [in]       scale const Vector3 & [=Vector3::UNIT_SCALE]    <TODO: insert parameter description here>
   void CollisionShape::getMeshInformation( const Ogre::Mesh* const mesh, size_t &vertex_count,
      Ogre::Vector3* &vertices,
      size_t &index_count, unsigned long* &indices,
      const Ogre::Vector3 &position,
      const Ogre::Quaternion &orient,
      const Ogre::Vector3 &scale)
   {
      bool added_shared = false;
      size_t current_offset = 0;
      size_t shared_offset = 0;
      size_t next_offset = 0;
      size_t index_offset = 0;


      vertex_count = index_count = 0;

      // Calculate how many vertices and indices we're going to need
      for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
      {
         Ogre::SubMesh* submesh = mesh->getSubMesh( i );

         // We only need to add the shared vertices once
         if(submesh->useSharedVertices)
         {
            if( !added_shared )
            {
               vertex_count += mesh->sharedVertexData->vertexCount;
               added_shared = true;
            }
         }
         else
         {
            vertex_count += submesh->vertexData->vertexCount;
         }

         // Add the indices
         index_count += submesh->indexData->indexCount;
      }


      // Allocate space for the vertices and indices
      vertices = new Ogre::Vector3[vertex_count];
      indices = new unsigned long[index_count];

      added_shared = false;

      // Run through the submeshes again, adding the data into the arrays
      for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
      {
         Ogre::SubMesh* submesh = mesh->getSubMesh(i);

         Ogre::VertexData* vertex_data = submesh->useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

         if((!submesh->useSharedVertices)||(submesh->useSharedVertices && !added_shared))
         {
            if(submesh->useSharedVertices)
            {
               added_shared = true;
               shared_offset = current_offset;
            }

            const Ogre::VertexElement* posElem =
               vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

            Ogre::HardwareVertexBufferSharedPtr vbuf =
               vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

            unsigned char* vertex =
               static_cast<unsigned char*>(vbuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));

            // There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
            //  as second argument. So make it float, to avoid trouble when Ogre::Real will
            //  be comiled/typedefed as double:
            //      Ogre::Real* pReal;
            float* pReal;

            for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
            {
               posElem->baseVertexPointerToElement(vertex, &pReal);

               Ogre::Vector3 pt(pReal[0], pReal[1], pReal[2]);

               vertices[current_offset + j] = (orient * (pt * scale)) + position;
            }

            vbuf->unlock();
            next_offset += vertex_data->vertexCount;
         }


         Ogre::IndexData* index_data = submesh->indexData;
         size_t numTris = index_data->indexCount / 3;
         Ogre::HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

         bool use32bitindexes = (ibuf->getType() == Ogre::HardwareIndexBuffer::IT_32BIT);

         unsigned long*  pLong = static_cast<unsigned long*>(ibuf->lock(Ogre::HardwareBuffer::HBL_READ_ONLY));
         unsigned short* pShort = reinterpret_cast<unsigned short*>(pLong);


         size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

         if ( use32bitindexes )
         {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
               indices[index_offset++] = pLong[k] + static_cast<unsigned long>(offset);
            }
         }
         else
         {
            for ( size_t k = 0; k < numTris*3; ++k)
            {
               indices[index_offset++] = static_cast<unsigned long>(pShort[k]) +
                  static_cast<unsigned long>(offset);
            }
         }

         ibuf->unlock();
         current_offset = next_offset;
      }
   }

   /// Get entity.
   /// @return Entity *
   Entity* CollisionShape::getEntity()
   {
      return mEntity;
   }

   /// <TODO: insert function description here>
   /// @param [in, out]  ent Entity *    <TODO: insert parameter description here>
   /// @return bool <TODO: insert return value description here>
   bool CollisionShape::Load(Entity* ent)
   {
      assert(ent);
      mEntity = ent;

      size_t vertex_count,index_count;
      Vector3* vertices;
      unsigned long* indices;
      numVertices = 0;
      numFaces = 0;

      
      getMeshInformation(mEntity->getMesh().getPointer(), vertex_count, vertices, index_count, indices);

      numFaces = index_count;
      numVertices = vertex_count;

      // read vertices, add triangles...
      Begin(numVertices, numFaces/3);

      Vector3 vec3;
      for(int i = 0; i < numVertices; i++)
      {
         SetVertex(i, vertices[i]);
      }
      for (int i = 0; i < numFaces; i++)
      {
         int i0 = *indices++;
         int i1 = *indices++;
         int i2 = *indices++;
         SetTriangle(i, i0, i1, i2);
      }


      // finish adding geometry
      End();
      
      return true;
   }
}