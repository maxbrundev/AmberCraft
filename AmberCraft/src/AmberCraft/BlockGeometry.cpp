#include "pch.h"

#include "AmberCraft/BlockGeometry.h"

std::vector<AmberCraft::BlockVertex> AmberCraft::BlockGeometry::m_vertices;
glm::vec3 AmberCraft::BlockGeometry::m_positions[8];
glm::vec3 AmberCraft::BlockGeometry::m_normals[6];
glm::vec2 AmberCraft::BlockGeometry::m_textureCoordinates[4];
bool AmberCraft::BlockGeometry::m_initialized = false;

void AmberCraft::BlockGeometry::Setup()
{
	if (!m_initialized)
	{
		m_initialized = true;
		InitCubePositions();
		InitCubeNormals();
		InitCubeTextureCoordinates();
		InitCubeVertices();
	}
}

void AmberCraft::BlockGeometry::InitCubePositions()
{
	m_positions[0] = glm::vec3(-0.5, +0.5, -0.5);
	m_positions[1] = glm::vec3(-0.5, +0.5, +0.5);
	m_positions[2] = glm::vec3(+0.5, +0.5, +0.5);
	m_positions[3] = glm::vec3(+0.5, +0.5, -0.5);
	m_positions[4] = glm::vec3(-0.5, -0.5, -0.5);
	m_positions[5] = glm::vec3(-0.5, -0.5, +0.5);
	m_positions[6] = glm::vec3(+0.5, -0.5, +0.5);
	m_positions[7] = glm::vec3(+0.5, -0.5, -0.5);
}

void AmberCraft::BlockGeometry::InitCubeTextureCoordinates()
{
	m_textureCoordinates[0] = glm::vec2(0, 0);
	m_textureCoordinates[1] = glm::vec2(0, 1);
	m_textureCoordinates[2] = glm::vec2(1, 0);
	m_textureCoordinates[3] = glm::vec2(1, 1);
}

void AmberCraft::BlockGeometry::InitCubeNormals()
{
	m_normals[0] = glm::vec3(-1, 0, 0);
	m_normals[1] = glm::vec3(1, 0, 0);
	m_normals[2] = glm::vec3(0, -1, 0);
	m_normals[3] = glm::vec3(0, 1, 0);
	m_normals[4] = glm::vec3(0, 0, -1);
	m_normals[5] = glm::vec3(0, 0, 1);
}

void AmberCraft::BlockGeometry::InitCubeVertices()
{
	CreateTriangle(3, 1, 2, 3, 0, 2, 3);
	CreateTriangle(3, 0, 1, 3, 1, 0, 3);

	CreateTriangle(2, 5, 6, 3, 0, 2, 5);
	CreateTriangle(2, 1, 5, 3, 1, 0, 5);

	CreateTriangle(4, 6, 5, 3, 0, 2, 2);
	CreateTriangle(4, 7, 6, 3, 1, 0, 2);

	CreateTriangle(0, 7, 4, 3, 0, 2, 4);
	CreateTriangle(0, 3, 7, 3, 1, 0, 4);

	CreateTriangle(1, 4, 5, 3, 0, 2, 0);
	CreateTriangle(1, 0, 4, 3, 1, 0, 0);

	CreateTriangle(3, 6, 7, 3, 0, 2, 1);
	CreateTriangle(3, 2, 6, 3, 1, 0, 1);
}

void AmberCraft::BlockGeometry::CreateTriangle(uint8_t pos1, uint8_t pos2, uint8_t pos3, uint8_t text1, uint8_t text2, uint8_t text3, uint8_t p_normals)
{
	m_vertices.emplace_back(m_positions[pos1], m_textureCoordinates[text1], m_normals[p_normals]);
	m_vertices.emplace_back(m_positions[pos2], m_textureCoordinates[text2], m_normals[p_normals]);
	m_vertices.emplace_back(m_positions[pos3], m_textureCoordinates[text3], m_normals[p_normals]);
}

std::vector< AmberCraft::BlockVertex>& AmberCraft::BlockGeometry::GetVertices()
{
	return m_vertices;
}