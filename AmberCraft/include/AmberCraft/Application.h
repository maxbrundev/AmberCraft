#pragma once

#include <AmberEngine/Managers/RenderingManager.h>

#include "AmberCraft/World.h"

namespace RenderEngine
{
	namespace Systems
	{
		struct RaycastCollision
		{
			bool isFound;
			AmberCraft::BlockType type;
			glm::vec3 collisionPoint;
			glm::vec3 blockPosition;
			glm::vec3 blockToCollisionPointDirection;
			glm::vec3 collisionFaceNormal;
		};

		class Application
		{
		public:
			Application();
			~Application() = default;
			void Setup();
			void Run();

			void RayCast(glm::vec3 p_source, glm::vec3 p_direction, float p_maxDistance, RaycastCollision& p_out) const;

		private:
			AmberEngine::Managers::RenderingManager m_renderingManager;
			AmberEngine::Resources::Shader m_shader;
			AmberEngine::Resources::Shader m_shaderOutline;
			AmberCraft::World m_world;
			bool disableShadows;
			GLuint m_vao{};
			GLuint m_vbo{};
			GLuint m_ssbo{};

		};
	}
}