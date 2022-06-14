#pragma once

#include "BlockData.h"
#include "ChunkBuffers.h"

#define CHUNK_SIZE 16
#define CHUNK_ELEMENTS_COUNT CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE

namespace AmberCraft
{
	class Chunk
	{
	public:
	private:
		enum class ChunkSides
		{
			LEFT,
			RIGHT,
			BOT,
			TOP,
			BACK,
			FRONT
		};

		struct ChunkNeighbors
		{
			Chunk* left		= nullptr;
			Chunk* right	= nullptr;
			Chunk* bot		= nullptr;
			Chunk* top		= nullptr;
			Chunk* back		= nullptr;
			Chunk* front	= nullptr;
		};

	public:
		Chunk(const glm::ivec3& p_position);

		Chunk(const Chunk& other)            = default;
		Chunk(Chunk&& other)                 = default;
		Chunk& operator=(const Chunk& other) = default;
		Chunk& operator=(Chunk&& other)      = default;

		~Chunk() = default;

		void Fill(BlockType p_blockType = BlockType::DIRT);
		void SetChunksNeighbors(Chunk* p_left, Chunk* p_right, Chunk* p_top, Chunk* p_bot, Chunk* p_front, Chunk* p_back);
		void UpdateNeighBors() const;

		bool IsBlockOccluded(uint8_t p_x, uint8_t p_y, uint8_t p_z);
		
		BlockData* GetBlock(uint8_t p_x, uint8_t p_y, uint8_t p_z, ChunkSides p_chunkSide);

		std::vector<uint32_t> FillBlocksToRender();

		void UpdateBlocksToRender();
		void Draw() const;

		bool IsOccluded() const;

		glm::ivec3 GetChunkCoordinatePosition() const;
		glm::ivec3 GetWorldCoordinatePosition() const;

		void SetPosition(const glm::ivec3& p_position);

		static std::array<uint8_t, 3> From1Dto3D(uint16_t p_index);
		static uint16_t From3Dto1D(uint8_t p_x, uint8_t p_y, uint8_t p_z);

	public:
		BlockData blocks[CHUNK_ELEMENTS_COUNT];

	private:
		ChunkBuffers m_chunkBuffers;
		ChunkNeighbors m_chunksNeighbors;

		glm::ivec3 m_chunkCoordinatePosition;

		uint16_t m_blocksToRenderCount;

		bool m_isOccluded;
	};
}