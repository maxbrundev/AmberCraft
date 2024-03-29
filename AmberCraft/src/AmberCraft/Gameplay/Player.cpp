#include "AmberCraftPCH.h"

#include "AmberCraft/Gameplay/Player.h"

#include "AmberCraft/Utils/ServiceLocator.h"

#include "AmberCraft/Terrain/World.h"

#include "AmberCraft/Physics/Physics.h"

AmberCraft::Gameplay::Player::Player(AmberEngine::Managers::RenderingManager& p_renderingManager, Core::Game& p_game) :
	m_renderingManager(p_renderingManager), m_game(p_game), m_currentBlock(Terrain::EBlockType::AIR)
{
}

void AmberCraft::Gameplay::Player::Update() const
{
	HandleInputs();
	Physics::RaycastCollision result;
	Physics::Physics::Raycast(m_renderingManager.GetCamera().GetPosition(), m_renderingManager.GetCamera().GetForward(), 10.0f, result);

	if (result.IsFound)
	{
		m_game.SetOutlineState(true);
		m_game.SetOutlinePosition(result.BlockPosition);
	}
	else
	{
		m_game.SetOutlineState(false);
	}
}

glm::vec3 AmberCraft::Gameplay::Player::GetPosition() const
{
	return m_renderingManager.GetCamera().GetPosition();
}

glm::vec3 AmberCraft::Gameplay::Player::GetForward() const
{
	return m_renderingManager.GetCamera().GetForward();
}

void AmberCraft::Gameplay::Player::SetCurrentBlock(Terrain::EBlockType p_blockType)
{
	m_currentBlock = p_blockType;
}

void AmberCraft::Gameplay::Player::HandleInputs() const
{
	if(ImGui::GetIO().WantCaptureMouse)
		return;

	Physics::RaycastCollision result{};

	if (m_renderingManager.GetInputManager().IsKeyPressed(0x1))
	{
		if (Physics::Physics::Raycast(m_renderingManager.GetCamera().GetPosition(), m_renderingManager.GetCamera().GetForward(), m_raycastDistance, result))
		{
			Utils::ServiceLocator::Get<Terrain::World>().SetBlock(result.BlockPosition.x, result.BlockPosition.y, result.BlockPosition.z, Terrain::BlockData{ Terrain::EBlockType::AIR }, true);
		}
	}

	if (m_renderingManager.GetInputManager().IsKeyPressed(0x2))
	{
		if (Physics::Physics::Raycast(m_renderingManager.GetCamera().GetPosition(), m_renderingManager.GetCamera().GetForward(), m_raycastDistance, result))
		{
			Utils::ServiceLocator::Get<Terrain::World>().SetBlock(result.BlockPosition.x + result.CollisionFaceNormal.x, result.BlockPosition.y + result.CollisionFaceNormal.y, result.BlockPosition.z + result.CollisionFaceNormal.z, Terrain::BlockData{ m_currentBlock }, true);
		}
	}
}
