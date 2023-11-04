#pragma once

namespace AmberCraft::Geometry
{
	struct BlockVertex
	{
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;

		BlockVertex(glm::vec3 p_position, glm::vec2 p_uv, glm::vec3 p_normal) :
			position(p_position),
			uv(p_uv),
			normal(p_normal)
		{}
	};

	struct BlockGeometry
	{
		static glm::vec3 m_positions[8];
		static glm::vec3 m_normals[6];
		static glm::vec2 m_uvs[4];

		static std::vector<BlockVertex> m_vertices;
		static void SetUp();
		static void InitializePositions();
		static void InitializeNormals();
		static void InitializeUVs();
		static void InitCubeVertices();
		static void CreateTriangle(uint8_t pos1, uint8_t pos2, uint8_t pos3, uint8_t text1, uint8_t text2, uint8_t text3, uint8_t p_normals);
		static void CreateFace(uint8_t pos1, uint8_t pos2, uint8_t pos3, uint8_t pos4, uint8_t text1, uint8_t text2, uint8_t text3, uint8_t text4, uint8_t p_normals);

		static std::vector<BlockVertex>& GetVertices();

		static bool m_initialized;
	};
}
