///////////////////////////////////////////////////////////////////////////////
///  @file IOgreCollisionShape.h
///  @brief Abstract base class for collision shapes
///
///  @author The OgreOpcode Team @date 01-29-2006
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
#ifndef __IOgreCollisionShape_h__
# define __IOgreCollisionShape_h__

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
	/// One ICollisionShape object may be shared between several
	/// CollisionObject%s. 2 ICollisionShape objects may also
	/// be queried directly whether they intersect.
	///
	/// ICollisionShape objects are also able to load themselves
	/// from a mesh file.
	class _OgreOpcode_Export ICollisionShape
	{
	public:
		/// Constructs a ICollisionShape
		ICollisionShape(const String& name) {};
		virtual ~ICollisionShape() {};

		/// visualize the collide shape
		virtual void visualize() = 0;
		/// toggle debug rendering.
		virtual void setDebug(bool debug) = 0;
		/// 
		virtual void setDynamic() = 0;
		virtual void setStatic() = 0;
		virtual void showAABB(bool showThis) = 0;
		/// return current center in world space
		virtual Vector3 getCenter() const = 0;
		/// return current center in object space
		virtual Vector3 getLocalCenter() const = 0;
		virtual String getName() const = 0;
		/// get radius of collide mesh
		virtual Real getRadius() const = 0;
		/// return the full transformation matrix
		virtual Matrix4 getFullTransform(void) const = 0;
		/// return the local transformation matrix
		virtual Matrix4 getLocalTransform(void) const = 0;
        virtual SceneNode* getParentSceneNode(void) const = 0;
		/// Retrieve current vertex data from mesh and refit collision tree.
		/// This is an O(n) operation in the number of vertices in the mesh.
		virtual bool refit() = 0;
		/// return current world space AABB min and max
		virtual void getMinMax(Vector3& bMin, Vector3& bMax) const = 0; 
		/// return current object space AABB min and max
		virtual void getLocalMinMax(Vector3& bMin, Vector3& bMax) const = 0; 
		/// perform collision with other ICollisionShape
		virtual bool collide(CollisionType collType, Matrix4& ownMatrix, ICollisionShape* otherShape, Matrix4& otherMatrix, CollisionPair& collPair) = 0;
		/// perform collision with line
		virtual bool rayCheck(CollisionType collType, const Matrix4& ownMatrix, const Ray& line, const Real dist, CollisionPair& collPair) = 0;
		/// perform a sphere check
		virtual bool sphereCheck(CollisionType collType, const Matrix4& ownMatrix, const Sphere& ball, CollisionPair& collPair) = 0;
	};
}; // namespace OgreOpcode

#endif // __IOgreCollisionShape_h__
