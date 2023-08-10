#include "AmberCraftPCH.h"

#include "AmberCraft/Utils/WorldUtils.h"

#include "AmberCraft/Terrain/Chunk.h"
#include "AmberCraft/Terrain/World.h"

glm::ivec3 AmberCraft::Utils::WorldUtils::GetWorldToChunkCoordinate(const glm::vec3& p_worldPosition)
{
	glm::ivec3 result = glm::round(p_worldPosition);

	result.x = static_cast<uint64_t>(result.x) / CHUNK_SIZE + WORLD_SIZE / 2;
	result.y = static_cast<uint64_t>(result.y) / CHUNK_SIZE + WORLD_SIZE / 2;
	result.z = static_cast<uint64_t>(result.z) / CHUNK_SIZE + WORLD_SIZE / 2;

	return result;
}

uint64_t AmberCraft::Utils::WorldUtils::GetWorldDistanceInChunkCoordinate(const glm::vec3& p_from, const glm::vec3& p_to)
{
	const glm::ivec3 from = GetWorldToChunkCoordinate(p_from);
	const glm::ivec3 to   = GetWorldToChunkCoordinate(p_to);

	return static_cast<uint8_t>(sqrt(pow(from.x - to.x, 2.0f) + pow(from.y - to.y, 2.0f) + pow(from.z - to.y, 2.0f)));
}
