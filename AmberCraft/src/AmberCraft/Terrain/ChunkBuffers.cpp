#include "AmberCraftPCH.h"

#include "AmberCraft/Terrain/ChunkBuffers.h"
#include "AmberCraft/Terrain/BlockGeometry.h"

AmberCraft::Terrain::ChunkBuffers::ChunkBuffers()
{
	InitBuffers();
}

AmberCraft::Terrain::ChunkBuffers::~ChunkBuffers()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ebo);
	glDeleteBuffers(1, &m_ssbo);
}

void AmberCraft::Terrain::ChunkBuffers::InitBuffers()
{
	BlockGeometry::SetUp();

	m_indices[0] = 0;
	m_indices[1] = 1;
	m_indices[2] = 2;
	m_indices[3] = 2;
	m_indices[4] = 3;
	m_indices[5] = 0;

	m_indices[6 ] = 4;
	m_indices[7 ] = 5;
	m_indices[8 ] = 6;
	m_indices[9 ] = 6;
	m_indices[10] = 7;
	m_indices[11] = 4;

	m_indices[12] = 8;
	m_indices[13] = 9;
	m_indices[14] = 10;
	m_indices[15] = 10;
	m_indices[16] = 11;
	m_indices[17] = 8;

	m_indices[18] = 12;
	m_indices[19] = 13;
	m_indices[20] = 14;
	m_indices[21] = 14;
	m_indices[22] = 15;
	m_indices[23] = 12;

	m_indices[24] = 16;
	m_indices[25] = 17;
	m_indices[26] = 18;
	m_indices[27] = 18;
	m_indices[28] = 19;
	m_indices[29] = 16;

	m_indices[30] = 20;
	m_indices[31] = 21;
	m_indices[32] = 22;
	m_indices[33] = 22;
	m_indices[34] = 23;
	m_indices[35] = 20;

	const auto& vertices = BlockGeometry::GetVertices();

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BlockVertex), vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &m_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, std::size(m_indices) * sizeof(int), m_indices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), reinterpret_cast<void*>(offsetof(BlockVertex, uv)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(BlockVertex), reinterpret_cast<void*>(offsetof(BlockVertex, normal)));

	glGenBuffers(1, &m_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void AmberCraft::Terrain::ChunkBuffers::SendBlocksToGPU(const std::vector<GLuint>& p_blocksToRender) const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, p_blocksToRender.size() * sizeof(GLuint), p_blocksToRender.data(), GL_DYNAMIC_DRAW);
	//GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	//memcpy(p, p_blocksToRender.data(), p_blocksToRender.size() * sizeof(GLuint));
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void AmberCraft::Terrain::ChunkBuffers::DrawChunk(uint16_t p_blocksToRenderCount) const
{
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_ssbo);

	glBindVertexArray(m_vao);

	glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(std::size(m_indices)), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(p_blocksToRenderCount));
	//glDrawArraysInstanced(GL_TRIANGLES, 0, 36, static_cast<GLsizei>(p_blocksToRenderCount));
	glBindVertexArray(0);
}
