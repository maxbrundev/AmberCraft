#pragma once

#include <AmberEngine/Managers/RenderingManager.h>

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

	private:
		AmberEngine::Managers::RenderingManager m_renderingManager;
		AmberEngine::Resources::Shader m_shader;
		AmberEngine::Resources::Shader m_shaderOutline;

		std::shared_ptr<Terrain::World> m_world;
		std::shared_ptr<Gameplay::Player> m_player;
		
		bool disableShadows;
		GLuint m_vao{};
		GLuint m_vbo{};
		GLuint m_ssbo{};

	};
}
