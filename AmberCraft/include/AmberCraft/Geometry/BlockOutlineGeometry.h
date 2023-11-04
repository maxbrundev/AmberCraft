#pragma once

namespace AmberCraft::Geometry
{
	class BlockOutlineGeometry
	{
	public:
		BlockOutlineGeometry();
		~BlockOutlineGeometry();

		void Draw() const;

	private:
		void InitializePositions();
		void InitializeIndices();
		void InitializeBuffers();

	private:
		GLuint m_vbo;
		GLuint m_vao;
		GLuint m_ebo;

		glm::vec3 m_positions[8];
		int m_indices[24];
	};
}