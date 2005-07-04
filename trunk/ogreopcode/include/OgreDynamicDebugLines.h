///////////////////////////////////////////////////////////////////////////////
///  @file OgreDynamicDebugLines.h
///  @brief <TODO: insert file description here>
///
///  @author jacmoe @date 28-06-2005
///  @remarks  Based directly on code from OgreODE, made by Ed "Green Eyed Monster" Jones.
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
#ifndef __OgreDynamicDebugLines_h__
#define __OgreDynamicDebugLines_h__

#include "OgreOpcodeExports.h"
#include <OgreSimpleRenderable.h>
#include <vector>

namespace OgreOpcode
{
   namespace Details
   {
      /// Abstract base class providing mechanisms for dynamically growing hardware buffers.
      class DynamicDebugRenderable : public Ogre::SimpleRenderable
      {
      public:
         /// Constructor
         DynamicDebugRenderable();
         /// Virtual destructor
         virtual ~DynamicDebugRenderable();

         /// Initializes the dynamic renderable.
         /// @remarks
         /// This function should only be called once. It initializes the
         /// render operation, and calls the abstract function
         /// createVertexDeclaration().
         /// @param operationType The type of render operation to perform.
         /// @param useIndices Specifies whether to use indices to determine the
         /// vertices to use as input.
         void initialize(Ogre::RenderOperation::OperationType operationType,
            bool useIndices);

         /// Implementation of Ogre::SimpleRenderable
         virtual Ogre::Real getBoundingRadius(void) const;
         /// Implementation of Ogre::SimpleRenderable
         virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera* cam) const;

      protected:
         /// Maximum capacity of the currently allocated vertex buffer.
         size_t mVertexBufferCapacity;
         /// Maximum capacity of the currently allocated index buffer.
         size_t mIndexBufferCapacity;

         /// Creates the vertex declaration.
         /// @remarks
         /// Override and set mRenderOp.vertexData->vertexDeclaration here.
         /// mRenderOp.vertexData will be created for you before this method
         /// is called.
         virtual void createVertexDeclaration() = 0;

         /// Prepares the hardware buffers for the requested vertex and index counts.
         /// @remarks
         /// This function must be called before locking the buffers in
         /// fillHardwareBuffers(). It guarantees that the hardware buffers
         /// are large enough to hold at least the requested number of
         /// vertices and indices (if using indices). The buffers are
         /// possibly reallocated to achieve this.
         /// @par
         /// The vertex and index count in the render operation are set to
         /// the values of vertexCount and indexCount respectively.
         /// @param vertexCount The number of vertices the buffer must hold.
         /// @param indexCount The number of indices the buffer must hold. This
         /// parameter is ignored if not using indices.
         void prepareHardwareBuffers(size_t vertexCount, size_t indexCount);

         /// Fills the hardware vertex and index buffers with data.
         /// @remarks
         /// This function must call prepareHardwareBuffers() before locking
         /// the buffers to ensure the they are large enough for the data to
         /// be written. Afterwards the vertex and index buffers (if using
         /// indices) can be locked, and data can be written to them.
         virtual void fillHardwareBuffers() = 0;
      };

      class DynamicDebugLines : public DynamicDebugRenderable
      {
         typedef Ogre::Vector3 Vector3;
         typedef Ogre::Quaternion Quaternion;
         typedef Ogre::Camera Camera;
         typedef Ogre::Real Real;
         typedef Ogre::RenderOperation::OperationType OperationType;

      public:
         /// Constructor - see setOperationType() for description of argument.
         DynamicDebugLines(OperationType opType=Ogre::RenderOperation::OT_LINE_STRIP);
         virtual ~DynamicDebugLines();

         /// Add a point to the point list
         void addPoint(const Ogre::Vector3 &p);
         /// Add a point to the point list
         void addPoint(Real x, Real y, Real z);

         /// Change the location of an existing point in the point list
         void setPoint(unsigned short index, const Vector3 &value);

         /// Return the location of an existing point in the point list
         const Vector3& getPoint(unsigned short index) const;

         /// Return the total number of points in the point list
         unsigned short getNumPoints(void) const;

         /// Remove all points from the point list
         void clear();

         /// Call this to update the hardware buffer after making changes.
         void update();

         /// Set the type of operation to draw with.
         /// * @param opType Can be one of
         /// *    - RenderOperation::OT_LINE_STRIP
         /// *    - RenderOperation::OT_LINE_LIST
         /// *    - RenderOperation::OT_POINT_LIST
         /// *    - RenderOperation::OT_TRIANGLE_LIST
         /// *    - RenderOperation::OT_TRIANGLE_STRIP
         /// *    - RenderOperation::OT_TRIANGLE_FAN
         /// *    The default is OT_LINE_STRIP.
         void setOperationType(OperationType opType);
         OperationType getOperationType() const;

      protected:
         /// Implementation DynamicDebugRenderable, creates a simple vertex-only decl
         virtual void createVertexDeclaration();
         /// Implementation DynamicDebugRenderable, pushes point list out to hardware memory
         virtual void fillHardwareBuffers();

      private:
         std::vector<Vector3> mPoints;
         bool mDirty;
      };

   }  // namespace Debug
} // namespace OgreOpcode

#endif // __OgreDynamicDebugLines_h__
