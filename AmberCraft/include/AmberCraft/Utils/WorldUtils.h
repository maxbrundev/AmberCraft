#pragma once

namespace AmberCraft::Utils
{
	class WorldUtils final
	{
	public:
		WorldUtils() = delete;

		static glm::ivec3 GetWorldToChunkCoordinate(const glm::vec3& p_worldPosition);

		static uint64_t GetWorldDistanceInChunkCoordinate(const glm::vec3& p_from, const glm::vec3& p_to);
	};
}
