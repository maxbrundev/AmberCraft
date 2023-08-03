#pragma once

namespace AmberCraft::Terrain
{
	class ChunkBuffers
	{
	public:
		ChunkBuffers();
		~ChunkBuffers() = default;

		void InitBuffers();
		void SendBlocksToGPU(const std::vector<GLuint>& p_blocksToRender) const;
		void DrawChunk(uint16_t p_blocksToRenderCount) const;

	private:
		GLuint m_vao{};
		GLuint m_vbo{};
		GLuint m_ssbo{};
	};
}