#pragma once

#include "AmberCraft/Terrain/EBlockType.h"

namespace AmberCraft::Physics
{
	struct RaycastCollision
	{
		bool IsFound;
		Terrain::EBlockType Type;
		glm::vec3 CollisionPoint;
		glm::vec3 BlockPosition;
		glm::vec3 BlockToCollisionPointDirection;
		glm::vec3 CollisionFaceNormal;
	};
}
