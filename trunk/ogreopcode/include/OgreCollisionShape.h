///////////////////////////////////////////////////////////////////////////////
///  @file OgreCollisionShape.h
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
#ifndef __OgreCollisionShape_h__
# define __OgreCollisionShape_h__

#include "OgreOpcodeExports.h"
# include <Ogre.h>

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
	class _OgreOpcode_Export CollisionShape
	{
	public:
		/// Constructs a CollisionShape
		CollisionShape(const String& name);

		virtual ~CollisionShape();

		/// has object been initialized?
		bool isInitialized();
		/// get radius of collide mesh
		virtual Real getRadius() const;
		/// load collide geometry from mesh, and build a collision tree
		virtual bool load(Entity* ent);
		/// Retrieve current vertex data from mesh and refit collision tree.
		/// This is an O(n) operation in the number of vertices in the mesh.
		virtual bool refit();
		/// Reload the collision geometry from mesh, rebuild collision tree from scratch. 
		/// Potentially very slow. Only necessary if the mesh has drastically changed,
		/// like topology changing deformations, or a change in the number of tris.
		/// In most cases RefitToMesh() is sufficient, and much faster.
		/// Under usual circumstances there is no need to call this method.
		virtual bool rebuild();
		/// Refits the collision tree to the currently cached vertex data.
		/// This is an O(n) operation in the number of vertices in the mesh.
		/// This is an advanced method.  It assumes that the user is manually 
		/// updating both the CollisionShape's cached data and the actual mesh
		/// hardware buffers.  Mostly useful for implementing something like 
		/// deformable body physics.
		virtual bool _refitToCachedData();
		/// rebuild collision tree from scratch using currently cached vertex data
		/// This is potentially quite slow.  Only necessary if the mesh has drastically changed,
		/// like topology changing deformations, or a change in the number of tris.
		/// In most cases _RefitToCachedGeometry() is sufficient, and much faster.
		/// This is an advanced method.  It assumes that the user is manually 
		/// updating both the CollisionShape's cached data and the actual mesh
		/// hardware buffers.  Mostly useful for implementing something like
		/// deformable body physics.
		virtual bool _rebuildFromCachedData();
		/// perform collision with other CollisionShape
		virtual bool collide(CollisionType collType, Matrix4& ownMatrix, CollisionShape* otherShape, Matrix4& otherMatrix, CollisionPair& collPair);
		/// perform collision with line
		virtual bool rayCheck(CollisionType collType, const Matrix4& ownMatrix, const Ray& line, const Real dist, CollisionPair& collPair);
		/// perform a sphere check
		virtual bool sphereCheck(CollisionType collType, const Matrix4& ownMatrix, const Sphere& ball, CollisionPair& collPair);
		/// visualize the collide shape
		virtual void visualize();
		/// toggle debug rendering.
		virtual void setDebug(bool debug);
		/// return entity
		virtual void setDynamic(bool iAmDynamic = true)
		{
			this->isDynamic = iAmDynamic;
		}

		/// return the full transformation matrix
		virtual Matrix4 getFullTransform(void) const;
		/// return the local transformation matrix
		virtual Matrix4 getLocalTransform(void) const;
        virtual SceneNode* getParentSceneNode(void) const;

		String getName() const;

		/// return current center in world space
		Vector3 getCenter() const;
		/// return current center in object space
		Vector3 getLocalCenter() const;
		/// return current world space AABB min and max
		void getMinMax(Vector3& bMin, Vector3& bMax) const; 
		/// return current object space AABB min and max
		void getLocalMinMax(Vector3& bMin, Vector3& bMax) const; 

	private:
		/// Count up the total number of vertices and indices in the Ogre mesh
		void countIndicesAndVertices(Entity * entity, size_t & index_count, size_t & vertex_count);
		/// Convert ogre Mesh to simple float and int arrays
		void convertMeshData(Entity * entity, float * vertexData, size_t vertex_count, int * faceData=0, size_t index_count=0);
		/// get tri coords from tri index
		void getTriCoords(int index, Vector3& v0, Vector3& v1, Vector3& v2);
		/// visualize the AABBTree of the opcode model
		void visualizeAABBCollisionNode(const Opcode::AABBCollisionNode* node);
		/// visualize the AABBTree of the opcode model (for no leaf trees)
		void visualizeAABBNoLeafNode(const Opcode::AABBNoLeafNode* node);
		void _prepareOpcodeCreateParams(Opcode::OPCODECREATE& opcc);

		void calculateSize();

		float   mRadius;

		Opcode::BVTCache*         opcTreeCache;
		Opcode::CollisionFaces*   opcFaceCache;

		Opcode::MeshInterface opcMeshAccess;
		Opcode::Model opcModel;
		int numVertices;
		int numFaces;
		float* mVertexBuf;
		int*   mFaceBuf;
		Entity* mEntity;
		SceneNode* mParentNode;
		String mName;
		bool mInitialized;
		bool isDynamic;
		int refCount;
		mutable Matrix4 mFullTransform;
		mutable Matrix4 mLocalTransform;
		/// prevent default construction
		CollisionShape();

		DebugObject* _debug_obj;
		void createDebugObject();
		void destroyDebugObject();
	};

	inline
		bool
		CollisionShape::isInitialized()
	{
		return mInitialized;
	}

	inline Matrix4 CollisionShape::getFullTransform(void) const
	{
		if(isDynamic)
		{
			mFullTransform = getParentSceneNode()->_getFullTransform();
		}
			return mFullTransform;
	}

	inline Matrix4 CollisionShape::getLocalTransform(void) const
	{
		if(isDynamic)
		{
			//TODO
		}
		return mLocalTransform;
	}

	inline SceneNode* CollisionShape::getParentSceneNode(void) const
	{
		return this->mParentNode;
	}

	//inline Entity* CollisionShape::getEntity()
	//{
	//	return mEntity;
	//}
	//inline const Entity* CollisionShape::getEntity() const
	//{
	//	return mEntity;
	//}

	inline
		Real
		CollisionShape::getRadius() const
	{
		return mRadius;
	}

	/// Extract triangle coordinates from triangle index.
	inline
		void
		CollisionShape::getTriCoords(int index, Vector3& v0, Vector3& v1, Vector3& v2)
	{
		int* indexPtr = &(mFaceBuf[3 * index]);
		float* vp0 = &(mVertexBuf[3 * indexPtr[0]]);
		float* vp1 = &(mVertexBuf[3 * indexPtr[1]]);
		float* vp2 = &(mVertexBuf[3 * indexPtr[2]]);
		v0 = Vector3(vp0[0], vp0[1], vp0[2]);
		v1 = Vector3(vp1[0], vp1[1], vp1[2]);
		v2 = Vector3(vp2[0], vp2[1], vp2[2]);
	}

}; // namespace OgreOpcode

#endif // __OgreCollisionEntity_h__
