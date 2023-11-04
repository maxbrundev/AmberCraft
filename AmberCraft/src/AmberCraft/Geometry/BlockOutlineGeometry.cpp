#include "AmberCraftPCH.h"

#include "AmberCraft/Geometry/BlockOutlineGeometry.h"

AmberCraft::Geometry::BlockOutlineGeometry::BlockOutlineGeometry()
{
	InitializePositions();
	InitializeIndices();
	InitializeBuffers();
}

AmberCraft::Geometry::BlockOutlineGeometry::~BlockOutlineGeometry()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
}

void AmberCraft::Geometry::BlockOutlineGeometry::Draw() const
{
	glBindVertexArray(m_vao);
	glDrawElements(GL_LINES, static_cast<GLsizei>(std::size(m_indices)), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void AmberCraft::Geometry::BlockOutlineGeometry::InitializePositions()
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

void AmberCraft::Geometry::BlockOutlineGeometry::InitializeIndices()
{
	// Front face
	m_indices[0] = 0;
	m_indices[1] = 1;

	m_indices[2] = 1;
	m_indices[3] = 2;
	
	m_indices[4] = 2;
	m_indices[5] = 3;

	m_indices[6] = 3;
	m_indices[7] = 0;

	// Back face
	m_indices[8]  = 6;
	m_indices[9]  = 5;

	m_indices[10] = 5;
	m_indices[11] = 4;

	m_indices[12] = 4;
	m_indices[13] = 7;

	m_indices[14] = 7;
	m_indices[15] = 6;

	// Top Right edge
	m_indices[16] = 0;
	m_indices[17] = 5;

	// Bottom Right edge
	m_indices[18] = 3;
	m_indices[19] = 4;

	// Bottom Left edge
	m_indices[20] = 2;
	m_indices[21] = 7;

	// Top Left edge
	m_indices[22] = 1;
	m_indices[23] = 6;
}

void AmberCraft::Geometry::BlockOutlineGeometry::InitializeBuffers()
{
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, std::size(m_positions) * sizeof(glm::vec3), m_positions, GL_STATIC_DRAW);

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, std::size(m_indices) * sizeof(int), m_indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
