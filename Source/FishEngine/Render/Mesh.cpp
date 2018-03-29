#include <FishEngine/Render/Mesh.hpp>
#include <FishEngine/Render/GLEnvironment.hpp>
#include <FishEngine/Math/IntVector.hpp>
#include <FishEngine/Debug.hpp>

#include <sstream>
#include <cassert>

constexpr int PositionIndex = 0;
constexpr int NormalIndex = 1;
constexpr int TangentIndex = 2;
constexpr int UVIndex = 3;
constexpr int BoneIndexIndex = 4;
constexpr int BoneWeightIndex = 5;

namespace FishEngine
{
	Mesh::Mesh(std::vector<Vector3>	&& vertices,
			   std::vector<Vector3>	&& normals,
			   std::vector<Vector2>	&& uv,
			   std::vector<Vector3>	&& tangents,
			   std::vector<uint32_t>	&& triangles)
		: Object(Mesh::ClassID, ClassName),
		m_vertices(vertices),
		m_normals(normals),
		m_uv(uv),
		m_tangents(tangents),
		m_triangles(triangles)
	{
		LOGF;
		m_vertexCount = static_cast<uint32_t>(vertices.size());
		m_triangleCount = static_cast<uint32_t>(triangles.size() / 3);
		m_subMeshIndexOffset.push_back(m_triangleCount);
		RecalculateBounds();
	}
	
//	Mesh* FromTextFile(std::istream & is)
	Mesh* Mesh::FromTextFile(const char* str)
	{
		std::istringstream is(str);
//		auto mesh = MakeShared<Mesh>();
		auto mesh = new Mesh();
		is >> mesh->m_vertexCount >> mesh->m_triangleCount;
		mesh->m_vertices.resize(mesh->m_vertexCount);
		mesh->m_normals.resize(mesh->m_vertexCount);
		mesh->m_uv.resize(mesh->m_vertexCount);
		mesh->m_tangents.resize(mesh->m_vertexCount);
		mesh->m_triangles.resize(mesh->m_triangleCount * 3);
		float vx, vy, vz;
		Vector3 vmin(Mathf::Infinity, Mathf::Infinity, Mathf::Infinity);
		Vector3 vmax(Mathf::NegativeInfinity, Mathf::NegativeInfinity, Mathf::NegativeInfinity);
		for (uint32_t i = 0; i < mesh->m_vertexCount; ++i)
		{
			is >> vx >> vy >> vz;
			if (vmin.x > vx) vmin.x = vx;
			if (vmin.y > vy) vmin.y = vy;
			if (vmin.z > vz) vmin.z = vz;
			if (vmax.x < vx) vmax.x = vx;
			if (vmax.y < vy) vmax.y = vy;
			if (vmax.z < vz) vmax.z = vz;
			auto & v = mesh->m_vertices[i];
			v.x = vx;
			v.y = vy;
			v.z = vz;
		}
		mesh->m_bounds.SetMinMax(vmin, vmax);
		for (auto & f : mesh->m_normals)
			is >> f.x >> f.y >> f.z;
		for (auto & f : mesh->m_uv)
			is >> f.x >> f.y;
		for (auto & f : mesh->m_tangents)
			is >> f.x >> f.y >> f.z;
		for (auto & f : mesh->m_triangles)
			is >> f;
		return mesh;
	}
	
	void Mesh::RecalculateBounds()
	{
		Vector3 bmin(Mathf::Infinity, Mathf::Infinity, Mathf::Infinity);
		Vector3 bmax(Mathf::NegativeInfinity, Mathf::NegativeInfinity, Mathf::NegativeInfinity);
		for (auto & v : m_vertices)
		{
			if (bmin.x > v.x)
				bmin.x = v.x;
			if (bmin.y > v.y)
				bmin.y = v.y;
			if (bmin.z > v.z)
				bmin.z = v.z;
			
			if (bmax.x < v.x)
				bmax.x = v.x;
			if (bmax.y < v.y)
				bmax.y = v.y;
			if (bmax.z < v.z)
				bmax.z = v.z;
		}
		m_bounds.SetMinMax(bmin, bmax);
	}
	
	
	void Mesh::UploadMeshData(bool markNoLogerReadable /*= true*/)
	{
		if (m_uploaded)
			return;
		GenerateBuffer();
		BindBuffer();
		glCheckError();
		
		//m_vertexCount = static_cast<uint32_t>(m_vertices.size());
		//m_triangleCount = static_cast<uint32_t>(m_triangles.size() / 3);
		m_isReadable = !markNoLogerReadable;
		if (markNoLogerReadable)
		{
			Clear();
		}
		m_uploaded = true;
	}
	
	void Mesh::Clear()
	{
		m_vertices.clear();
		m_vertices.shrink_to_fit();
		m_normals.clear();
		m_normals.shrink_to_fit();
		m_uv.clear();
		m_uv.shrink_to_fit();
		m_tangents.clear();
		m_tangents.shrink_to_fit();
		m_triangles.clear();
		m_triangles.shrink_to_fit();
		//m_boneIndexBuffer.clear();
		//m_boneWeightBuffer.clear();
		m_boneWeights.clear();
		m_boneWeights.shrink_to_fit();
	}
	
	
	//void Model::renderPatch(const Shader &shader) {
	//    glBindVertexArray(m_VAO);
	//    glDrawElements(GL_PATCHES, (GLsizei)m_triangles.size(), GL_UNSIGNED_INT, 0);
	//    glBindVertexArray(0);
	//}
	
	void Mesh::GenerateBuffer()
	{
		// VAO
		assert(m_VAO == 0);
		glGenVertexArrays(1, &m_VAO);
		
		// index VBO
		glGenBuffers(1, &m_indexVBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_triangles.size() * 4, m_triangles.data(), GL_STATIC_DRAW);
		
		glGenBuffers(1, &m_positionVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
		glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * 3 * 4, m_vertices.data(), GL_STATIC_DRAW);
		
		glGenBuffers(1, &m_normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size() * 3 * 4, m_normals.data(), GL_STATIC_DRAW);
		
		glGenBuffers(1, &m_uvVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_uvVBO);
		glBufferData(GL_ARRAY_BUFFER, m_uv.size() * 2 * 4, m_uv.data(), GL_STATIC_DRAW);
		
		//float* tangents = new float[m_tangents.size() * 4];
		//for (int i = 0; i < m_tangents.size(); ++i)
		//{
		//	Vector3 const & n = m_normals[i];
		//	Vector3 const & t = m_tangents[i];
		//}
		glGenBuffers(1, &m_tangentVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_tangentVBO);
		glBufferData(GL_ARRAY_BUFFER, m_tangents.size() * 3 * 4, m_tangents.data(), GL_STATIC_DRAW);
		
		if (m_skinned)
		{
			std::vector<Int4> boneIndexBuffer;
			std::vector<Vector4> boneWeightBuffer;
			boneIndexBuffer.reserve(m_boneWeights.size());
			boneIndexBuffer.reserve(m_boneWeights.size());
			for (auto const & b : m_boneWeights)
			{
				boneIndexBuffer.emplace_back(b.boneIndex[0], b.boneIndex[1], b.boneIndex[2], b.boneIndex[3]);
				boneWeightBuffer.emplace_back(b.weight[0], b.weight[1], b.weight[2], b.weight[3]);
			}
			
			glGenTransformFeedbacks(1, &m_TFBO);
			
			glGenVertexArrays(1, &m_animationInputVAO);
			
			glGenBuffers(1, &m_animationOutputPositionVBO);
			glBindBuffer(GL_ARRAY_BUFFER, m_animationOutputPositionVBO);
			glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
			
			glGenBuffers(1, &m_animationOutputNormalVBO);
			glBindBuffer(GL_ARRAY_BUFFER, m_animationOutputNormalVBO);
			glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
			
			glGenBuffers(1, &m_animationOutputTangentVBO);
			glBindBuffer(GL_ARRAY_BUFFER, m_animationOutputTangentVBO);
			glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * 3 * sizeof(GLfloat), NULL, GL_DYNAMIC_DRAW);
			
			glGenBuffers(1, &m_boneIndexVBO);
			glBindBuffer(GL_ARRAY_BUFFER, m_boneIndexVBO);
			glBufferData(GL_ARRAY_BUFFER, boneIndexBuffer.size() * 4 * sizeof(int), boneIndexBuffer.data(), GL_STATIC_DRAW);
			
			glGenBuffers(1, &m_boneWeightVBO);
			glBindBuffer(GL_ARRAY_BUFFER, m_boneWeightVBO);
			glBufferData(GL_ARRAY_BUFFER, boneWeightBuffer.size() * 4 * sizeof(GLfloat), boneWeightBuffer.data(), GL_STATIC_DRAW);
			
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
	}
	
	void Mesh::BindBuffer()
	{
		if (m_skinned)
		{
			// Transform feedback input
			glBindVertexArray(m_animationInputVAO);
			
			// position
			glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
			glVertexAttribPointer(PositionIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(PositionIndex);
			
			// normal
			glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO);
			glVertexAttribPointer(NormalIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(NormalIndex);
			
			// tangent
			glBindBuffer(GL_ARRAY_BUFFER, m_tangentVBO);
			glVertexAttribPointer(TangentIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(TangentIndex);
			
			// bone indices
			glBindBuffer(GL_ARRAY_BUFFER, m_boneIndexVBO);
			glVertexAttribIPointer(BoneIndexIndex, 4, GL_INT, 4 * sizeof(GLint), (GLvoid*)0);
			glEnableVertexAttribArray(BoneIndexIndex);
			
			// bone weights
			glBindBuffer(GL_ARRAY_BUFFER, m_boneWeightVBO);
			glVertexAttribPointer(BoneWeightIndex, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
			glEnableVertexAttribArray(BoneWeightIndex);
			
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindVertexArray(0);
			
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, m_TFBO);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_animationOutputPositionVBO);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, m_animationOutputNormalVBO);
			glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, m_animationOutputTangentVBO);
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
		}
		
		glBindVertexArray(m_VAO);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexVBO);
		
		if (m_skinned)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_animationOutputPositionVBO);
		}
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_positionVBO);
		}
		glVertexAttribPointer(PositionIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(PositionIndex);
		
		
		if (m_skinned)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_animationOutputNormalVBO);
		}
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO);
		}
		//glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO);
		glVertexAttribPointer(NormalIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(NormalIndex);
		
		glBindBuffer(GL_ARRAY_BUFFER, m_uvVBO);
		glVertexAttribPointer(UVIndex, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(UVIndex);
		
		if (m_skinned)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_animationOutputTangentVBO);
		}
		else
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_tangentVBO);
		}
		//glBindBuffer(GL_ARRAY_BUFFER, m_tangentVBO);
		glVertexAttribPointer(TangentIndex, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
		glEnableVertexAttribArray(TangentIndex);
		
		glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind
		
		glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
	}
	
	void Mesh::Render( int subMeshIndex /* = -1*/)
	{
		//assert(m_uploaded);
		if (!m_uploaded)
		{
			UploadMeshData();
		}
		
		glBindVertexArray(m_VAO);
		
		if (subMeshIndex < 0 && subMeshIndex != -1)
		{
			LogWarning(Format( "invalid subMeshIndex {}", subMeshIndex ));
			subMeshIndex = -1;
		}
		else if (subMeshIndex >= m_subMeshCount)
		{
			//Debug::LogWarning("invalid subMeshIndex %d", subMeshIndex);
			subMeshIndex = m_subMeshCount;
		}
		
		if (subMeshIndex == -1 || m_subMeshCount == 1)
		{
			glDrawElements(GL_TRIANGLES, m_triangleCount * 3, GL_UNSIGNED_INT, 0);
		}
		else
		{
			GLvoid * offset = (GLvoid *)( m_subMeshIndexOffset[subMeshIndex] * sizeof(GLuint) );
			int index_count = 0;
			if (subMeshIndex == m_subMeshCount-1) // the last one
			{
				index_count = m_triangleCount * 3 - m_subMeshIndexOffset[m_subMeshCount-1];
				//index_count = 0;
			}
			else
			{
				index_count = m_subMeshIndexOffset[subMeshIndex+1] - m_subMeshIndexOffset[subMeshIndex];
			}
			glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, offset);
		}
		
		glBindVertexArray(0);
	}
}
