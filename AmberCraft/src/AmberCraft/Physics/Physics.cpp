#include "AmberCraftPCH.h"

#include "AmberCraft/Physics/Physics.h"

#include "AmberCraft/Utils/ServiceLocator.h"

#include "AmberCraft/Terrain/World.h"

bool AmberCraft::Physics::Physics::Raycast(const glm::vec3& p_source, const glm::vec3& p_direction, float p_maxDistance, RaycastCollision& p_out)
{
	RaycastCollision& result = p_out;

	result.IsFound             = false;
	result.Type                = Terrain::EBlockType::AIR;
	result.CollisionPoint      = glm::vec3(0.0f);
	result.BlockPosition       = glm::vec3(0.0f);
	result.CollisionFaceNormal = glm::vec3(0.0f);

	const int raycastSteps = 1000.0f;

	for (int currentStep = 0; currentStep < raycastSteps && !result.IsFound; currentStep++)
	{
		const glm::vec3 raycastHitPoint = p_source + p_direction * p_maxDistance * (static_cast<float>(currentStep) / static_cast<float>(raycastSteps));
		const glm::vec3 raycastBlockPosition = glm::round(raycastHitPoint);

		result.Type = Utils::ServiceLocator::Get<Terrain::World>().GetBlock(raycastBlockPosition.x, raycastBlockPosition.y, raycastBlockPosition.z).type;

		if(result.Type == Terrain::EBlockType::AIR)
			continue;

		result.IsFound         = true;
		result.CollisionPoint  = raycastHitPoint;
		result.BlockPosition.x = raycastBlockPosition.x;
		result.BlockPosition.y = raycastBlockPosition.y;
		result.BlockPosition.z = raycastBlockPosition.z;

		result.BlockToCollisionPointDirection = glm::normalize(result.CollisionPoint - result.BlockPosition);

		const glm::vec3 absoluteDirectionFromBlockToImpact = glm::abs(result.BlockToCollisionPointDirection);

		if (absoluteDirectionFromBlockToImpact.x > absoluteDirectionFromBlockToImpact.y && absoluteDirectionFromBlockToImpact.x > absoluteDirectionFromBlockToImpact.z)
		{
			result.CollisionFaceNormal = result.BlockToCollisionPointDirection.x > 0.0f ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(-1.0f, 0.0f, 0.0f);
		}
		else if (absoluteDirectionFromBlockToImpact.y > absoluteDirectionFromBlockToImpact.x && absoluteDirectionFromBlockToImpact.y > absoluteDirectionFromBlockToImpact.z)
		{
			result.CollisionFaceNormal = result.BlockToCollisionPointDirection.y > 0.0f ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(0.0f, -1.0f, 0.0f);
		}
		else
		{
			result.CollisionFaceNormal = result.BlockToCollisionPointDirection.z > 0.0f ? glm::vec3(0.0f, 0.0f, 1.0f) : glm::vec3(0.f, 0.f, -1.f);
		}
	}

	return result.IsFound;
}
