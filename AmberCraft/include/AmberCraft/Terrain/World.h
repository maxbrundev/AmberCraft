#pragma once

#include <AmberEngine/Managers/RenderingManager.h>

#include "AmberCraft/Terrain/TerrainLayerData.h"

#include "AmberCraft/Noise/NoiseData.h"

#include "AmberCraft/Noise/FastNoise.h"

namespace AmberCraft::Terrain
{
	class Chunk;
}

namespace AmberCraft::Noise
{
	class PerlinNoise;
}

constexpr int WORLD_SIZE = 10;
constexpr int WORLD_ELEMENTS_COUNT = WORLD_SIZE * WORLD_SIZE * WORLD_SIZE;

namespace AmberCraft::Terrain
{
	struct ShiftWorldDirectionData
	{
		std::pair<uint8_t, int8_t> Direction;
		uint8_t Count;

		explicit ShiftWorldDirectionData(const std::pair<uint8_t, int8_t>& p_shiftDirection, uint8_t p_shiftState = 0) :
		Direction(std::move(p_shiftDirection)),
		Count(p_shiftState)
		{ }
	};

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

		void GenerateTerrain() const;
		void GenerateChunk(uint8_t p_xChunk, uint8_t p_zChunk, bool p_update = false) const;
		void ApplyTerrainLayers(int64_t p_worldX, int16_t p_height, int64_t p_worldZ, bool p_update = false) const;

		void SetNeighbors() const;
		void SetNeighbor(uint8_t p_x, uint8_t p_y, uint8_t p_z) const;

		void Draw(AmberEngine::Managers::RenderingManager& p_renderingManager) const;

		void UpdateChunksFromPlayerPosition(const glm::vec3& p_playerPosition, const glm::vec3& p_playerForwad);

		constexpr int64_t LocalToWorldX(int64_t p_localX) const;
		constexpr int64_t LocalToWorldZ(int64_t p_localZ) const;
		uint8_t HeightAt(int16_t p_x, int16_t p_y) const;
		
		void SetShiftWorldDirection(const glm::vec2& p_direction);
	
		void ShiftWorld(uint8_t p_axis, int8_t p_shiftDirection);

		void UpdateChunksToRender() const;
		
		Chunk* CreateChunk(const glm::vec3& p_position) const;
		void DestroyChunk(uint16_t p_index);

		BlockData GetBlock(int16_t p_x, int16_t p_y, int16_t p_z) const;
		void SetBlock(int16_t p_x, int16_t p_y, int16_t p_z, BlockData p_blockData, bool p_updateChunk) const;

		static constexpr uint16_t From3Dto1D(uint8_t p_x, uint8_t p_y, uint8_t p_z);
		static constexpr std::array<uint8_t, 3> From1Dto3D(uint16_t p_index);

		static std::array<int64_t, 3> GetChunkCoordinates(const glm::vec3& p_worldPosition);

	public:
		static bool __WORLD_IFINITE_GENERATION;
		int64_t m_xChunkOffset;
		int64_t m_zChunkOffset;
	private:
		std::vector<Chunk*> m_chunks;
		std::unique_ptr<Noise::PerlinNoise> m_perlin;
		FastNoise m_fastNoise;
		Noise::NoiseData m_noiseData;
		bool isChunkPosChanged = false;
		

		std::vector<TerrainLayer> m_terrainLayers;
		
		std::queue<ShiftWorldDirectionData> m_shiftWorldQueue;
	};
}
