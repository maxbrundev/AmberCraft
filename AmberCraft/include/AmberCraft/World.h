#pragma once

#include "Chunk.h"
#include "AmberEngine/Managers/RenderingManager.h"

#define WORLD_SIZE 20
#define WORLD_ELEMENTS_COUNT WORLD_SIZE * WORLD_SIZE * WORLD_SIZE

namespace AmberCraft
{
	class World
	{
	private:
		std::vector<Chunk> m_chunks;

	public:
		World();
		~World() = default;

		void SetNeighbors();

		bool IsInWorld(uint8_t p_index);

		void GenerateTerrain();

		// UpdateWorld Calculate Surface Culling and send data to GPU
		void Update();
		void Draw(AmberEngine::Managers::RenderingManager& p_renderingManager);

		BlockData GetBlock(uint64_t p_x, uint64_t p_y, uint64_t p_z);
		bool SetBlock(uint64_t p_x, uint64_t p_y, uint64_t p_z, BlockData p_blockData, bool p_updateChunk = false);

		static std::array<uint8_t, 3> From1Dto3D(uint16_t p_index);
		static uint16_t From3Dto1D(uint8_t p_x, uint8_t p_y, uint8_t p_z);
	};
}