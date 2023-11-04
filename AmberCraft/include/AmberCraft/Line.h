#pragma once

class Line
{
public:
	Line(const glm::vec3& p_startPoint, const glm::vec3& p_endPoint);
	~Line();

	void Draw() const;

private:
	glm::vec3 m_startPoint;
	glm::vec3 m_endPoint;

	GLuint m_vbo;
	GLuint m_vao;

	glm::vec3 m_positions[2];
};