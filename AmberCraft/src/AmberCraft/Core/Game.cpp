#include "AmberCraftPCH.h"

#include "AmberCraft/Core/Game.h"

#include "AmberCraft/Terrain/World.h"

#include "AmberCraft/Gameplay/Player.h"

#include "AmberCraft/Utils/ServiceLocator.h"

AmberCraft::Core::Game::Game() : disableShadows(false)
{
	m_world = std::make_shared<Terrain::World>();

	m_player = std::make_unique<Gameplay::Player>(m_renderingManager);

	Utils::ServiceLocator::Provide(*m_world);

	m_debugUI = std::make_unique<UI::DebugUI>(*m_world, *m_player);

	m_shader = m_renderingManager.GetResourcesManager().LoadShaderFiles("chunk", "chunk.vs", "chunk.fs");
	m_shader.Bind();
	m_shader.SetUniform1i("disableShadows", disableShadows);
	m_shader.SetUniformVec3("skyColour", glm::vec3(0.5, 0.5, 0.5));
	m_shader.Unbind();

	//Temporary Outline implementation
	/*m_shaderOutline = m_renderingManager.GetResourcesManager().LoadShaderFiles("outline", "outline.vs", "outline.fs");
	AmberCraft::BlockGeometry::Setup();
	const auto& vertices = AmberCraft::BlockGeometry::GetVertices();
	
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(AmberCraft::BlockVertex), vertices.data(), GL_STATIC_DRAW);
	
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(AmberCraft::BlockVertex), nullptr);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(AmberCraft::BlockVertex), reinterpret_cast<void*>(offsetof(AmberCraft::BlockVertex, textureCoord)));
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);*/
}

void AmberCraft::Core::Game::Initialize()
{
	AmberEngine::Resources::Texture& texture = m_renderingManager.GetResourcesManager().LoadTexture("atlasBlocks", "blocks.png");
}

void AmberCraft::Core::Game::Run()
{
	m_world->GenerateTerrain();

	while (m_renderingManager.IsRunning())
	{
		m_renderingManager.Clear();
		m_renderingManager.Update();

		if (m_renderingManager.GetInputManager().IsKeyEventOccured(0x46))
		{
			disableShadows = !disableShadows;
			m_shader.Bind();
			m_shader.SetUniform1i("disableShadows", disableShadows);
		}

		m_player->Update();

		m_world->Draw(m_renderingManager);

		m_debugUI->Draw();

		//Temporary Outline implementation
		/*float raycastDistance = 10;
		RaycastCollision result;
		RayCast(playerPosition, playerForward, raycastDistance, result);

		if (result.isFound)
		{
			const glm::mat4 projectionMatrix = m_renderingManager.CalculateProjectionMatrix();
			const glm::mat4 viewMatrix = m_renderingManager.CalculateViewMatrix();
		
			m_shaderOutline.Bind();
			m_shaderOutline.SetUniformMat4("model", glm::translate(glm::mat4(1.0f), result.blockPosition));
			m_shaderOutline.SetUniformMat4("view", viewMatrix);
			m_shaderOutline.SetUniformMat4("projection", projectionMatrix);
		
			glBindVertexArray(m_vao);
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
		}*/

		m_renderingManager.SwapBuffers();
	}
}