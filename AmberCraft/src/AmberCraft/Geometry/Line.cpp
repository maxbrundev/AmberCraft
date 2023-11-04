#include "AmberCraftPCH.h"

#include "AmberCraft/Geometry/Line.h"

AmberCraft::Geometry::Line::Line(const glm::vec3& p_startPoint, const glm::vec3& p_endPoint) : m_startPoint(p_startPoint), m_endPoint(p_endPoint)
{
	m_positions[0] = m_startPoint;
	m_positions[1] = m_endPoint;

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, std::size(m_positions) * sizeof(glm::vec3), m_positions, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

AmberCraft::Geometry::Line::~Line()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
}

void AmberCraft::Geometry::Line::Draw() const
{
	glBindVertexArray(m_vao);
	glDrawArrays(GL_LINES, 0, 2);
	glBindVertexArray(0);
}
