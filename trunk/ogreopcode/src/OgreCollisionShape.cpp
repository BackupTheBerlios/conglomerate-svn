///////////////////////////////////////////////////////////////////////////////
///  @file OgreCollisionShape.cpp
///  @brief <TODO: insert file description here>
///
///  @author The OgreOpcode Team @date 28-05-2005
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
#include "OgreOpcodeExports.h"
#include "OgreCollisionShape.h"
#include "OgreCollisionReporter.h"
#include "OgreCollisionManager.h"
#include "OgreOpcodeMath.h"
#include "OgreOpcodeUtils.h"

namespace OgreOpcode
{
	//------------------------------------------------------------------------
	MeshCollisionShape::MeshCollisionShape(const String& name)
		: ICollisionShape(name)
	{
	}

	//------------------------------------------------------------------------
	MeshCollisionShape::~MeshCollisionShape()
	{
		if (mEntity && mEntity->hasSkeleton())
		{
			mEntity->removeSoftwareAnimationRequest(false);
		}
		delete[] mVertexBuf;
		delete[] mFaceBuf;
	}

	//------------------------------------------------------------------------
	///////////////////////////////////////////////////////////////////////////////
	/// Counts how many indices (faces) and vertices an entity contains.
	/// @param[in]  entity Entity to count its data.
	/// @param[out] index_count  Number of indices.
	/// @param[out] vertex_count Number of vertices.
	/// @author Yavin from the Ogre4J team
	///////////////////////////////////////////////////////////////////////////////
	void MeshCollisionShape::countIndicesAndVertices(Entity * entity, size_t & index_count, size_t & vertex_count)
	{
		Mesh * mesh = entity->getMesh().getPointer();

		bool hwSkinning = entity->isHardwareAnimationEnabled();

		bool added_shared = false;
		index_count  = 0;
		vertex_count = 0;

		// Calculate how many vertices and indices we're going to need
		for ( unsigned short i = 0; i < mesh->getNumSubMeshes(); ++i)
		{
			SubMesh* submesh = mesh->getSubMesh( i );

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
	}


	//------------------------------------------------------------------------
	//////////////////////////////////////////////////////////////////////////
	/// Converts mesh vertex and face data into simple float arrays.
	/// If the buffer parameters are null then that data is not converted.
	/// @param[in]  entity              Entity to extract data from.
	/// @param[out] vertexBuf          Target vertex data array (can be null).
	/// @param[in]  size_t vertex_count Number of vertices.
	/// @param[out] faceData            Target face data array (can be null).
	/// @param[int] index_count         Number of indices.
	/// @author Yavin from the Ogre4J team
	//////////////////////////////////////////////////////////////////////////
	void MeshCollisionShape::convertMeshData(Entity * entity,
		float * vertexBuf, size_t vertex_count,
		int * faceBuf, size_t index_count)
	{
		//---------------------------------------------------------------------
		// CONVERT MESH DATA
		//---------------------------------------------------------------------
		MeshPtr mesh = entity->getMesh();
		bool added_shared = false;
		size_t current_offset = 0;
		size_t shared_offset = 0;
		size_t next_offset = 0;
		size_t index_offset = 0;
		int numOfSubs = 0;

		bool useSoftwareBlendingVertices = entity->hasSkeleton();

		if (useSoftwareBlendingVertices)
		{
			entity->_updateAnimation();
		}

		// Run through the submeshes again, adding the data into the arrays
		for ( size_t i = 0; i < mesh->getNumSubMeshes(); ++i)
		{
			SubMesh* submesh = mesh->getSubMesh(i);
			bool useSharedVertices = submesh->useSharedVertices;

			if (vertexBuf)
			{
				//----------------------------------------------------------------
				// GET VERTEXDATA
				//----------------------------------------------------------------
				const VertexData * vertex_data;
				if(useSoftwareBlendingVertices)
					vertex_data = useSharedVertices ? entity->_getSkelAnimVertexData() : entity->getSubEntity(i)->_getSkelAnimVertexData();
				else
					vertex_data = useSharedVertices ? mesh->sharedVertexData : submesh->vertexData;

				if((!useSharedVertices)||(useSharedVertices && !added_shared))
				{
					if(useSharedVertices)
					{
						added_shared = true;
						shared_offset = current_offset;
					}

					const VertexElement* posElem =
						vertex_data->vertexDeclaration->findElementBySemantic(Ogre::VES_POSITION);

					HardwareVertexBufferSharedPtr vbuf =
						vertex_data->vertexBufferBinding->getBuffer(posElem->getSource());

					unsigned char* vertex =
						static_cast<unsigned char*>(vbuf->lock(HardwareBuffer::HBL_READ_ONLY));

					// There is _no_ baseVertexPointerToElement() which takes an Ogre::Real or a double
					//  as second argument. So make it float, to avoid trouble when Ogre::Real is
					//  comiled/typedefed as double:
					float* pReal;

					for( size_t j = 0; j < vertex_data->vertexCount; ++j, vertex += vbuf->getVertexSize())
					{
						posElem->baseVertexPointerToElement(vertex, &pReal);

						size_t n = current_offset*3 + j*3;

						vertexBuf[n + 0] = pReal[0];
						vertexBuf[n + 1] = pReal[1];
						vertexBuf[n + 2] = pReal[2];
					}

					vbuf->unlock();
					next_offset += vertex_data->vertexCount;
				}
			}

			if (faceBuf)
			{
				//----------------------------------------------------------------
				// GET INDEXDATA
				//----------------------------------------------------------------
				IndexData* index_data = submesh->indexData;
				size_t numTris = index_data->indexCount / 3;
				HardwareIndexBufferSharedPtr ibuf = index_data->indexBuffer;

				bool use32bitindexes = (ibuf->getType() == HardwareIndexBuffer::IT_32BIT);

				uint32 *pLong = static_cast<uint32*>(ibuf->lock(HardwareBuffer::HBL_READ_ONLY));
				uint16* pShort = reinterpret_cast<uint16*>(pLong);


				size_t offset = (submesh->useSharedVertices)? shared_offset : current_offset;

				if ( use32bitindexes )
				{
					for ( size_t k = 0; k < numTris*3; ++k)
					{
						faceBuf[index_offset++] = pLong[k] + static_cast<int>(offset);
					}
				}
				else
				{
					for ( size_t k = 0; k < numTris*3; ++k)
					{
						faceBuf[index_offset++] = static_cast<int>(pShort[k]) + static_cast<int>(offset);
					}
				}

				ibuf->unlock();
			}

			current_offset = next_offset;
		}
	}

	//------------------------------------------------------------------------
	/// <TODO: insert function description here>
	/// @param [in, out]  ent Entity *    <TODO: insert parameter description here>
	/// @return bool <TODO: insert return value description here>
	bool MeshCollisionShape::load(Entity* ent)
	{
		assert(ent);
		assert(!mVertexBuf && !mFaceBuf);
		mEntity = ent;

		if (mEntity->hasSkeleton()) {
			mEntity->addSoftwareAnimationRequest(false);
		}

		mParentNode = mEntity->getParentSceneNode();
		//mFullTransform = mEntity->getParentSceneNode()->_getFullTransform();
		mParentNode->getWorldTransforms(&mFullTransform);
		return rebuild();
	}

	//------------------------------------------------------------------------
	/// <TODO: insert function description here>
	/// @return bool <TODO: insert return value description here>
	bool MeshCollisionShape::rebuild()
	{
		assert(mEntity);

		// NOTE: Assuming presence or absence of skeleton hasn't changed!

		size_t vertex_count = 0;
		size_t index_count  = 0;
		countIndicesAndVertices(mEntity, index_count, vertex_count);

		// Re-Allocate space for the vertices and indices
		if (mVertexBuf && numVertices != vertex_count) {
			delete [] mVertexBuf;
			mVertexBuf = 0;
		}
		if (mFaceBuf && numFaces != index_count/3) {
			delete [] mFaceBuf;
			mFaceBuf = 0;
		}

		if (!mVertexBuf)
			mVertexBuf = new float[vertex_count * 3];
		if (!mFaceBuf)
			mFaceBuf = new int[index_count];

		convertMeshData(mEntity, mVertexBuf, vertex_count, mFaceBuf, index_count );

		numFaces = index_count / 3;
		numVertices = vertex_count;

		opcMeshAccess.SetNbTriangles(numFaces);
		opcMeshAccess.SetNbVertices(numVertices);
		opcMeshAccess.SetPointers((IceMaths::IndexedTriangle*)mFaceBuf, (IceMaths::Point*)mVertexBuf);
		opcMeshAccess.SetStrides(sizeof(int) * 3, sizeof(float) * 3);

		return _rebuildFromCachedData();

	}

	//------------------------------------------------------------------------
	/// <TODO: insert function description here>
	/// @return bool <TODO: insert return value description here>
	bool MeshCollisionShape::refit()
	{
		// bail if we don't need to refit
		if ( mShapeIsStatic )
			return true;

		assert(mEntity && mVertexBuf);

#ifdef _DEBUG
		size_t vertex_count = 0;
		size_t index_count  = 0;
		countIndicesAndVertices(mEntity, index_count, vertex_count);
		assert(numVertices == vertex_count);
#endif

		convertMeshData(mEntity, mVertexBuf, numVertices);

		return _refitToCachedData();
	}


	//------------------------------------------------------------------------
	/// <TODO: insert function description here>
	/// @return bool <TODO: insert return value description here>
	bool MeshCollisionShape::_refitToCachedData()
	{
		assert(mEntity && mVertexBuf);

		// rebuild tree
		if (!opcModel.Refit())
		{
			LogManager::getSingleton().logMessage(
				"OgreOpcode::MeshCollisionShape::_refitToCachedData(): OPCODE Quick refit not possible with the given tree type.");
			// Backup plan -- rebuild full tree
			opcMeshAccess.SetPointers((IceMaths::IndexedTriangle*)mFaceBuf, (IceMaths::Point*)mVertexBuf);
			Opcode::OPCODECREATE opcc;
			_prepareOpcodeCreateParams(opcc);
			opcModel.Build(opcc);
		}

		calculateSize();

		if (_debug_obj) {
			setDebug(true);
		}

		return true;
	}

	//------------------------------------------------------------------------
	/// <TODO: insert function description here>
	/// @return bool <TODO: insert return value description here>
	bool MeshCollisionShape::_rebuildFromCachedData()
	{
		assert(mEntity && mVertexBuf && mFaceBuf);

		Opcode::OPCODECREATE opcc;
		_prepareOpcodeCreateParams(opcc);
		opcModel.Build(opcc);

		calculateSize();

		if (_debug_obj) {
			setDebug(true);
		}

		return true;
	}
}

//------------------------------------------------------------------------
