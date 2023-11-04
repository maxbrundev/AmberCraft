#pragma once

#include "IPanel.h"

#include "AmberCraft/Terrain/EBlockType.h"

namespace AmberCraft::Gameplay
{
	class Player;
}

namespace AmberCraft::Terrain
{
	class World;
}

namespace AmberCraft::UI
{
	class DebugUI final : IPanel
	{
	public:
		DebugUI(Terrain::World& p_world, Gameplay::Player& p_player);
		~DebugUI() override = default;

		void Draw() override;
		void Initialize() override {}

	private:
		Terrain::World& m_world;
		Gameplay::Player& m_player;
		const std::string m_title;
		Terrain::EBlockType m_currenBlockType;

		std::string m_currentBlockType;
	};
}
