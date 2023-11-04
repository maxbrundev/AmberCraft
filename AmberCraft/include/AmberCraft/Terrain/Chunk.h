#pragma once

#include "AmberCraft/Terrain/BlockData.h"
#include "AmberCraft/Terrain/ChunkBuffers.h"

constexpr int CHUNK_SIZE = 16;
constexpr int CHUNK_ELEMENTS_COUNT = CHUNK_SIZE * CHUNK_SIZE * CHUNK_SIZE;

namespace AmberCraft::Terrain
{
	class Chunk
	{
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
		Chunk(const glm::vec3& p_position);
		~Chunk() = default;

		void Fill(EBlockType p_blockType = EBlockType::DIRT);
		void SetChunksNeighbors(Chunk* p_left, Chunk* p_right, Chunk* p_top, Chunk* p_bot, Chunk* p_front, Chunk* p_back);
		void UpdateNeighBors() const;
		
		bool IsBlockOccluded(uint8_t p_x, uint8_t p_y, uint8_t p_z);
		
		BlockData* GetBlock(uint8_t p_x, uint8_t p_y, uint8_t p_z, ChunkSides p_chunkSide);

		void FillBlocksToRender();

		void UpdateBlocksToRender();
		void Draw() const;

		bool IsOccluded() const;

		glm::vec3 GetPosition() const;

		static constexpr uint16_t From3Dto1D(uint8_t p_x, uint8_t p_y, uint8_t p_z);
		static constexpr std::array<uint8_t, 3> From1Dto3D(uint16_t p_index);
		

	public:
		BlockData blocks[CHUNK_ELEMENTS_COUNT];

		static bool __CHUNK_SURFACE_CULLING;
		static bool __BLOCK_SURFACE_CULLING;

	private:
		ChunkBuffers m_chunkBuffers;
		ChunkNeighbors m_chunksNeighbors;

		glm::vec3 m_chunkCoordinatePosition;
		std::vector<uint32_t> m_blocksToRender;
		uint16_t m_blocksToRenderCount;

		bool m_isOccluded;
	};
}