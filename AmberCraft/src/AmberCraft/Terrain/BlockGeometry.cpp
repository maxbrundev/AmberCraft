#include "AmberCraftPCH.h"

#include "AmberCraft/Terrain/BlockGeometry.h"

std::vector<AmberCraft::Terrain::BlockVertex> AmberCraft::Terrain::BlockGeometry::m_vertices;
glm::vec3 AmberCraft::Terrain::BlockGeometry::m_positions[8];
glm::vec3 AmberCraft::Terrain::BlockGeometry::m_normals[6];
glm::vec2 AmberCraft::Terrain::BlockGeometry::m_uvs[4];
bool AmberCraft::Terrain::BlockGeometry::m_initialized = false;


void AmberCraft::Terrain::BlockGeometry::SetUp()
{
	if (!m_initialized)
	{
		m_initialized = true;
		InitializePositions();
		InitializeNormals();
		InitializeUVs();
		InitCubeVertices();
	}
}

void AmberCraft::Terrain::BlockGeometry::InitializePositions()
{
	//    v6----- v5
	//   /|      /|
	//  v1------v0|
	//  | |     | |
	//  | |v7---|-|v4
	//  |/      |/
	//  v2------v3

	// Front face
	m_positions[0] = { 0.5f,  0.5f, 0.5f };
	m_positions[1] = { -0.5f,  0.5f, 0.5f };
	m_positions[2] = { -0.5f, -0.5f, 0.5f };
	m_positions[3] = { 0.5f, -0.5f, 0.5f };
	// Back Face
	m_positions[4] = { 0.5f,  -0.5f, -0.5f };
	m_positions[5] = { 0.5f,  0.5f, -0.5f };
	m_positions[6] = { -0.5f, 0.5f, -0.5f };
	m_positions[7] = { -0.5f, -0.5f, -0.5f };
}

void AmberCraft::Terrain::BlockGeometry::InitializeNormals()
{
	m_normals[0] = glm::vec3(1, 0, 0);
	m_normals[1] = glm::vec3(0, 1, 0);
	m_normals[2] = glm::vec3(0, 0, 1);
	m_normals[3] = glm::vec3(-1, 0, 0);
	m_normals[4] = glm::vec3(0, -1, 0);
	m_normals[5] = glm::vec3(0, 0, -1);
}

void AmberCraft::Terrain::BlockGeometry::InitializeUVs()
{
	m_uvs[0] = glm::vec2(0, 0);
	m_uvs[1] = glm::vec2(0, 1);
	m_uvs[2] = glm::vec2(1, 0);
	m_uvs[3] = glm::vec2(1, 1);
}

void AmberCraft::Terrain::BlockGeometry::InitCubeVertices()
{
	//Front
	//CreateTriangle(0, 1, 2, 3, 1, 0, 2);
	//CreateTriangle(2, 3, 0, 0, 2, 3, 2);

	//Right
	//CreateTriangle(5, 0, 3, 3, 1, 0, 0);
	//CreateTriangle(3, 4, 5, 0, 2, 3, 0);

	//Top
	//CreateTriangle(5, 6, 1, 3, 1, 0, 1);
	//CreateTriangle(1, 0, 5, 0, 2, 3, 1);

	//Left
	//CreateTriangle(1, 6, 7, 3, 1, 0, 3);
	//CreateTriangle(7, 2, 1, 0, 2, 3, 3);

	//Bottom
	//CreateTriangle(7, 4, 3, 3, 1, 0, 4);
	//CreateTriangle(3, 2, 7, 0, 2, 3, 4);

	//Back
	//CreateTriangle(6, 5, 4, 3, 1, 0, 5);
	//CreateTriangle(4, 7, 6, 0, 2, 3, 5);

	CreateFace(0, 1, 2, 3, 3, 1, 0, 2, 2);

	CreateFace(5, 0, 3, 4, 3, 1, 0, 2, 0);

	CreateFace(5, 6, 1, 0, 3, 1, 0, 2, 1);

	CreateFace(1, 6, 7, 2, 3, 1, 0, 2, 3);

	CreateFace(7, 4, 3, 2, 3, 1, 0, 2, 4);

	CreateFace(6, 5, 4, 7, 3, 1, 0, 2, 5);


}

void AmberCraft::Terrain::BlockGeometry::CreateTriangle(uint8_t pos1, uint8_t pos2, uint8_t pos3, uint8_t text1, uint8_t text2, uint8_t text3, uint8_t p_normals)
{
	m_vertices.emplace_back(m_positions[pos1], m_uvs[text1], m_normals[p_normals]);
	m_vertices.emplace_back(m_positions[pos2], m_uvs[text2], m_normals[p_normals]);
	m_vertices.emplace_back(m_positions[pos3], m_uvs[text3], m_normals[p_normals]);
}

void AmberCraft::Terrain::BlockGeometry::CreateFace(uint8_t pos1, uint8_t pos2, uint8_t pos3, uint8_t pos4, uint8_t text1, uint8_t text2, uint8_t text3, uint8_t text4, uint8_t p_normals)
{
	m_vertices.emplace_back(m_positions[pos1], m_uvs[text1], m_normals[p_normals]);
	m_vertices.emplace_back(m_positions[pos2], m_uvs[text2], m_normals[p_normals]);
	m_vertices.emplace_back(m_positions[pos3], m_uvs[text3], m_normals[p_normals]);
	m_vertices.emplace_back(m_positions[pos4], m_uvs[text4], m_normals[p_normals]);
}

std::vector< AmberCraft::Terrain::BlockVertex>& AmberCraft::Terrain::BlockGeometry::GetVertices()
{
	return m_vertices;
}
