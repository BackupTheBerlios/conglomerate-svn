///////////////////////////////////////////////////////////////////////////////
///  @file OgreDynamicDebugLines.cpp
///  @brief <TODO: insert file description here>
///
///  @author jacmoe @date 28-06-2005
///  @remarks  
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
#include "OgreDynamicDebugLines.h"
#include <OgreCamera.h>
#include <OgreHardwareBufferManager.h>

using namespace Ogre;

namespace OgreOpcode
{
   namespace Details
   {
      DynamicDebugRenderable::DynamicDebugRenderable()
      {
      }

      DynamicDebugRenderable::~DynamicDebugRenderable()
      {
         delete mRenderOp.vertexData;
         delete mRenderOp.indexData;
      }

      void DynamicDebugRenderable::initialize(RenderOperation::OperationType operationType,
         bool useIndices)
      {
         // Initialize render operation
         mRenderOp.operationType = operationType;
         mRenderOp.useIndexes = useIndices;
         mRenderOp.vertexData = new VertexData;
         if (mRenderOp.useIndexes)
            mRenderOp.indexData = new IndexData;

         // Reset buffer capacities
         mVertexBufferCapacity = 0;
         mIndexBufferCapacity = 0;

         // Create vertex declaration
         createVertexDeclaration();
      }

      void DynamicDebugRenderable::prepareHardwareBuffers(size_t vertexCount,
         size_t indexCount)
      {
         // Prepare vertex buffer
         size_t newVertCapacity = mVertexBufferCapacity;
         if ((vertexCount > mVertexBufferCapacity) ||
            (!mVertexBufferCapacity))
         {
            // vertexCount exceeds current capacity!
            // It is necessary to reallocate the buffer.

            // Check if this is the first call
            if (!newVertCapacity)
               newVertCapacity = 1;

            // Make capacity the next power of two
            while (newVertCapacity < vertexCount)
               newVertCapacity <<= 1;
         }
         else if (vertexCount < mVertexBufferCapacity>>1) {
            // Make capacity the previous power of two
            while (vertexCount < newVertCapacity>>1)
               newVertCapacity >>= 1;
         }
         if (newVertCapacity != mVertexBufferCapacity)
         {
            mVertexBufferCapacity = newVertCapacity;
            // Create new vertex buffer
            HardwareVertexBufferSharedPtr vbuf =
               HardwareBufferManager::getSingleton().createVertexBuffer(
               mRenderOp.vertexData->vertexDeclaration->getVertexSize(0),
               mVertexBufferCapacity,
               HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY); // TODO: Custom HBU_?

            // Bind buffer
            mRenderOp.vertexData->vertexBufferBinding->setBinding(0, vbuf);
         }
         // Update vertex count in the render operation
         mRenderOp.vertexData->vertexCount = vertexCount;

         if (mRenderOp.useIndexes)
         {
            OgreAssert(indexCount <= std::numeric_limits<unsigned short>::max(), "indexCount exceeds 16 bit");

            size_t newIndexCapacity = mIndexBufferCapacity;
            // Prepare index buffer
            if ((indexCount > newIndexCapacity) ||
               (!newIndexCapacity))
            {
               // indexCount exceeds current capacity!
               // It is necessary to reallocate the buffer.

               // Check if this is the first call
               if (!newIndexCapacity)
                  newIndexCapacity = 1;

               // Make capacity the next power of two
               while (newIndexCapacity < indexCount)
                  newIndexCapacity <<= 1;

            }
            else if (indexCount < newIndexCapacity>>1)
            {
               // Make capacity the previous power of two
               while (indexCount < newIndexCapacity>>1)
                  newIndexCapacity >>= 1;
            }

            if (newIndexCapacity != mIndexBufferCapacity)
            {
               mIndexBufferCapacity = newIndexCapacity;
               // Create new index buffer
               mRenderOp.indexData->indexBuffer =
                  HardwareBufferManager::getSingleton().createIndexBuffer(
                  HardwareIndexBuffer::IT_16BIT,
                  mIndexBufferCapacity,
                  HardwareBuffer::HBU_DYNAMIC_WRITE_ONLY); // TODO: Custom HBU_?
            }

            // Update index count in the render operation
            mRenderOp.indexData->indexCount = indexCount;
         }
      }

      Real DynamicDebugRenderable::getBoundingRadius(void) const
      {
         return Math::Sqrt(std::max(mBox.getMaximum().squaredLength(), mBox.getMinimum().squaredLength()));
      }

      Real DynamicDebugRenderable::getSquaredViewDepth(const Camera* cam) const
      {
         Vector3 vMin, vMax, vMid, vDist;
         vMin = mBox.getMinimum();
         vMax = mBox.getMaximum();
         vMid = ((vMin - vMax) * 0.5) + vMin;
         vDist = cam->getDerivedPosition() - vMid;

         return vDist.squaredLength();
      }

      enum
      {
         POSITION_BINDING,
         TEXCOORD_BINDING
      };

      DynamicDebugLines::DynamicDebugLines(OperationType opType)
      {
         initialize(opType,false);
         setMaterial("BaseWhiteNoLighting");
         mDirty = true;
      }

      DynamicDebugLines::~DynamicDebugLines()
      {
      }

      void DynamicDebugLines::setOperationType(OperationType opType)
      {
         mRenderOp.operationType = opType;
      }

      RenderOperation::OperationType DynamicDebugLines::getOperationType() const
      {
         return mRenderOp.operationType;
      }

      void DynamicDebugLines::addPoint(const Vector3 &p)
      {
         mPoints.push_back(p);
         mDirty = true;
      }
      void DynamicDebugLines::addPoint(Real x, Real y, Real z)
      {
         mPoints.push_back(Vector3(x,y,z));
         mDirty = true;
      }
      const Vector3& DynamicDebugLines::getPoint(unsigned short index) const
      {
         assert(index < mPoints.size() && "Point index is out of bounds!!");
         return mPoints[index];
      }
      unsigned short DynamicDebugLines::getNumPoints(void) const
      {
         return (unsigned short)mPoints.size();
      }
      void DynamicDebugLines::setPoint(unsigned short index, const Vector3 &value)
      {
         assert(index < mPoints.size() && "Point index is out of bounds!!");

         mPoints[index] = value;
         mDirty = true;
      }
      void DynamicDebugLines::clear()
      {
         mPoints.clear();
         mDirty = true;
      }

      void DynamicDebugLines::update()
      {
         if (mDirty) fillHardwareBuffers();
      }

      void DynamicDebugLines::createVertexDeclaration()
      {
         VertexDeclaration *decl = mRenderOp.vertexData->vertexDeclaration;
         decl->addElement(POSITION_BINDING, 0, VET_FLOAT3, VES_POSITION);
      }

      void DynamicDebugLines::fillHardwareBuffers()
      {
         int size = mPoints.size();

         prepareHardwareBuffers(size,0);

         if (!size) {
            mBox.setExtents(Vector3::ZERO,Vector3::ZERO);
            mDirty=false;
            return;
         }

         Vector3 vaabMin = mPoints[0];
         Vector3 vaabMax = mPoints[0];

         HardwareVertexBufferSharedPtr vbuf =
            mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);

         Real *prPos = static_cast<Real*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
         {
            for(int i = 0; i < size; i++)
            {
               *prPos++ = mPoints[i].x;
               *prPos++ = mPoints[i].y;
               *prPos++ = mPoints[i].z;

               if(mPoints[i].x < vaabMin.x)
                  vaabMin.x = mPoints[i].x;
               if(mPoints[i].y < vaabMin.y)
                  vaabMin.y = mPoints[i].y;
               if(mPoints[i].z < vaabMin.z)
                  vaabMin.z = mPoints[i].z;

               if(mPoints[i].x > vaabMax.x)
                  vaabMax.x = mPoints[i].x;
               if(mPoints[i].y > vaabMax.y)
                  vaabMax.y = mPoints[i].y;
               if(mPoints[i].z > vaabMax.z)
                  vaabMax.z = mPoints[i].z;
            }
         }
         vbuf->unlock();

         mBox.setExtents(vaabMin, vaabMax);

         mDirty = false;
      }

   }  // namespace Debug
}  // namespace OgreOpcode
