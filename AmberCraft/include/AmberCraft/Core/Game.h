#pragma once

#include <AmberEngine/Managers/RenderingManager.h>

#include "AmberCraft/Line.h"
#include "AmberCraft/Terrain/BlockOutlineGeometry.h"
#include "AmberCraft/UI/DebugUI.h"

namespace AmberCraft
{
	namespace Gameplay
	{
		class Player;
	}

	namespace Terrain
	{
		class World;
	}
}

namespace AmberCraft::Core
{
	class Game
	{
	public:
		Game();
		~Game() = default;

		void Initialize();
		void Run();

		void SetOutlinePosition(const glm::vec3 p_position);
		void SetOutlineState(bool p_state);

	private:
		AmberEngine::Managers::RenderingManager m_renderingManager;
		AmberEngine::Resources::Shader* m_currentShader;
		BlockOutlineGeometry outlineGeometry;

		std::shared_ptr<Terrain::World> m_world;
		std::shared_ptr<Gameplay::Player> m_player;

		std::unique_ptr<UI::DebugUI> m_debugUI;
		bool m_outlineIsEnabled;
		bool disableShadows;
		GLuint m_vao{};
		GLuint m_vbo{};
		GLuint m_ssbo{};

		Line m_lineX;
		Line m_lineY;
		Line m_lineZ;
	};
}
