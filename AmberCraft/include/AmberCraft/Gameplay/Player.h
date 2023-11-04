#pragma once

#include <AmberEngine/Managers/RenderingManager.h>

#include "AmberCraft/Core/Game.h"

#include "AmberCraft/Terrain/EBlockType.h"

namespace AmberCraft::Gameplay
{
	class Player
	{
	public:
		Player(AmberEngine::Managers::RenderingManager& p_renderingManager, Core::Game& p_game);
		~Player() = default;

		void Update() const;

		glm::vec3 GetPosition() const;
		glm::vec3 GetForward() const;

		void SetCurrentBlock(Terrain::EBlockType p_blockType);

	private:
		void HandleInputs() const;

	private:
		AmberEngine::Managers::RenderingManager& m_renderingManager;
		Core::Game& m_game;
		float m_raycastDistance = 10.0f;

		Terrain::EBlockType m_currentBlock;
	};
}
