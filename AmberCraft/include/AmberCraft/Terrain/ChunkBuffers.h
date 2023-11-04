#pragma once

namespace AmberCraft::Terrain
{
	class ChunkBuffers
	{
	public:
		ChunkBuffers();
		~ChunkBuffers();

		void InitBuffers();
		void SendBlocksToGPU(const std::vector<GLuint>& p_blocksToRender) const;
		void DrawChunk(uint16_t p_blocksToRenderCount) const;

	private:
		GLuint m_vao{};
		GLuint m_vbo{};
		GLuint m_ebo;
		GLuint m_ssbo{};
		int m_indices[36];
	};
}