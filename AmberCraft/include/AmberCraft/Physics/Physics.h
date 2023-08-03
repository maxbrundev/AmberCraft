#pragma once

#include "AmberCraft/Physics/RaycastCollision.h"

namespace AmberCraft::Physics
{
	class Physics
	{
	public:
		Physics() = delete;

		static bool Raycast(const glm::vec3& p_source, const glm::vec3& p_direction, float p_maxDistance, RaycastCollision& p_out);
	};
}
