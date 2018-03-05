#pragma once

#include <vector>
#include <map>
#include <cstdint>

//#include <FishEngine/Render/Mesh.hpp>
#include <FishEngine/Math/Vector3.hpp>
#include <FishEngine/Math/Vector2.hpp>

namespace FishEngine
{
	class Mesh;
	/**
	 * Raw mesh data used to construct optimized runtime rendering streams.
	 *
	 * A note on terminology. Information is stored at various frequencies as defined here:
	 *     Face - A single polygon in the mesh. Currently all code assumes this is a triangle but
	 *            conceptually any polygon would do.
	 *     Corner - Each face has N corners. As all faces are currently triangles, N=3.
	 *     Wedge - Properties stored for each corner of each face. Index with FaceIndex * NumCorners + CornerIndex.
	 *     Vertex - Properties shared by overlapping wedges of adjacent polygons. Typically these properties
	 *              relate to position. Index with VertexIndices[WedgeIndex].
	 *
	 * Additionally, to ease in backwards compatibility all properties should use only primitive types!
	 */
	class RawMesh
	{
	public:

		uint32_t m_vertexCount = 0;
		uint32_t m_faceCount = 0;

		/** Position in local space. Array[VertexId] = float3(x,y,z) */
		std::vector<Vector3> m_vertexPositions;

		/** Index of the vertex at this wedge. Array[WedgeId] = VertexId */
		std::vector<uint32_t> m_wedgeIndices;

		std::vector<Vector3> m_wedgeNormals;

		std::vector<Vector3> m_wedgeTangents;

		std::vector<Vector2> m_wedgeTexCoords;

		// polygonId to sub mesh id
		int					m_subMeshCount = 1;
		std::vector<int>	m_submeshMap;
		//std::vector<std::vector<int>> m_submeshPolygonIds;

		// disjoint set
		std::map<uint32_t, uint32_t> m_vertexIndexRemapping;

		void SetVertexCount(uint32_t vertexCount)
		{
			m_vertexCount = vertexCount;
			m_vertexPositions.reserve(vertexCount);
		}

		void SetFaceCount(uint32_t faceCount)
		{
			m_faceCount = faceCount;
			m_wedgeIndices.reserve(faceCount * 3);    // 3 corners
			m_wedgeNormals.reserve(faceCount * 3);
			m_wedgeTangents.reserve(faceCount * 3);
			m_wedgeTexCoords.reserve(faceCount * 2);
		}


		Mesh* ToMesh();
	};
}
