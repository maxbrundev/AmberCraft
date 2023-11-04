#include "AmberCraftPCH.h"

#include "AmberCraft/Core/Game.h"

#include "AmberCraft/Terrain/World.h"

#include "AmberCraft/Gameplay/Player.h"

#include "AmberCraft/Utils/ServiceLocator.h"

AmberCraft::Core::Game::Game() : disableShadows(false),
m_lineX({0.0f, 0.0f, 0.0f}, {0.2f, 0.0f, 0.0f}),
m_lineY({0.0f, 0.0f, 0.0f}, {0.0f, 0.2f, 0.0f}),
m_lineZ({0.0f, 0.0f, 0.0f}, { 0.0f, 0.0f, 0.2f })
{
	m_world = std::make_shared<Terrain::World>();

	Utils::ServiceLocator::Provide(*m_world);

	m_currentShader = &m_renderingManager.GetResourcesManager().LoadShaderFiles("Chunk", "Chunk.vs", "Chunk.fs");
	m_currentShader->Bind();
	m_currentShader->SetUniform1i("disableShadows", disableShadows);
	m_currentShader->SetUniformVec3("skyColor", glm::vec3(0.5, 0.5, 0.5));
	m_currentShader->Unbind();

	m_renderingManager.GetResourcesManager().LoadShader("Outline", "Outline.glsl");
	m_renderingManager.GetResourcesManager().LoadShader("Unlit", "Unlit.glsl");

	m_player = std::make_unique<Gameplay::Player>(m_renderingManager, *this);

	m_debugUI = std::make_unique<UI::DebugUI>(*m_world, *m_player);

	SetOutlineState(false);
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

		const glm::mat4 projectionMatrix = m_renderingManager.CalculateProjectionMatrix();
		const glm::mat4 viewMatrix = m_renderingManager.CalculateViewMatrix();

		if (m_renderingManager.GetInputManager().IsKeyEventOccured(0x46))
		{
			disableShadows = !disableShadows;
			m_currentShader->Bind();
			m_currentShader->SetUniform1i("disableShadows", disableShadows);
		}

		m_player->Update();
		m_world->UpdateChunksFromPlayerPosition(m_player->GetPosition(), m_player->GetForward());

		m_world->Draw(m_renderingManager);
		
		if(m_outlineIsEnabled)
		{
			if (const auto shader = &m_renderingManager.GetResourcesManager().GetShader("Outline"))
			{
				shader->Bind();
				shader->SetUniformMat4("u_Projection", projectionMatrix);
				shader->SetUniformMat4("u_View", viewMatrix);
				outlineGeometry.Draw();
				shader->Unbind();
			}
		}

		//Todo: draw instance axis lines
		glClear(GL_DEPTH_BUFFER_BIT);

		if (const auto shader = &m_renderingManager.GetResourcesManager().GetShader("Unlit"))
		{
			shader->Bind();
			shader->SetUniformMat4("projection", projectionMatrix);
			shader->SetUniformMat4("view", viewMatrix);
			shader->SetUniformMat4("model", glm::translate(glm::mat4(1.0f), m_renderingManager.GetCamera().GetPosition() + m_renderingManager.GetCamera().GetForward() * 3.0f));
			shader->SetUniformVec3("color", glm::vec3(1.0f, 0.0f, 0.0f));
			m_lineX.Draw();
			shader->SetUniformVec3("color", glm::vec3(0.0f, 1.0f, 0.0f));
			m_lineY.Draw();
			shader->SetUniformVec3("color", glm::vec3(0.0f, 0.0f, 1.0f));
			m_lineZ.Draw();
			shader->Unbind();
		}

		m_debugUI->Draw();

		m_renderingManager.SwapBuffers();
	}
}

void AmberCraft::Core::Game::SetOutlinePosition(const glm::vec3 p_position)
{
	const auto shader = &m_renderingManager.GetResourcesManager().GetShader("Outline");
	shader->Bind();
	shader->SetUniformMat4("u_Model", glm::translate(glm::mat4(1.0f), p_position));
	shader->Unbind();
}

void AmberCraft::Core::Game::SetOutlineState(bool p_state)
{
	m_outlineIsEnabled = p_state;
}
