#pragma once

#include <AmberEngine/Managers/RenderingManager.h>

#include "Chunk.h"

#define WORLD_SIZE 10
#define WORLD_ELEMENTS_COUNT WORLD_SIZE * WORLD_SIZE * WORLD_SIZE

namespace AmberCraft
{
	enum class EShiftDirection
	{
		NORTH,
		SOUTH,
		EST,
		WEST
	};

	class World
	{
	public:
		World();
		~World();
		void UpdateChuncksFromPlayerPosition(const glm::vec3& p_playerPosition);
		void RemoveChunckAwayFromPlayer(const glm::vec3& p_playerPosition);
		void SetNeighbors();
		bool IsInWorld(uint8_t p_index);
		void AddChunck(Chunk* p_chunck);
		void GenerateTerrain() const;
		void SetShiftWorldDirection(const glm::vec2& p_direction);
		void Shift(EShiftDirection p_direction);
		void UpdateChuncksToRender() const;
		void Draw(AmberEngine::Managers::RenderingManager& p_renderingManager) const;

		std::vector<Chunk*>& GetChunks() { return m_chunks; }

		Chunk* GetChunk(uint64_t p_x, uint64_t p_y, uint64_t p_z) const;
		BlockData GetBlock(uint64_t p_x, uint64_t p_y, uint64_t p_z) const;
		bool SetBlock(uint64_t p_x, uint64_t p_y, uint64_t p_z, BlockData p_blockData, bool p_updateChunk = false) const;
		
		static std::array<uint8_t, 3> From1Dto3D(uint16_t p_index);
		static uint16_t From3Dto1D(uint8_t p_x, uint8_t p_y, uint8_t p_z);
		static std::array<uint64_t, 3> PositionToChunkCoordinate(uint64_t p_x, uint64_t p_y, uint64_t p_z);

	private:
		std::vector<Chunk*> m_chunks;

		int m_offsetX = 0;
		int m_offsetZ = 0;
	};
}