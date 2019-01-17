#include "pch.h"

#include "AmberEngine/ImGui/imgui.h"

#include "AmberCraft/Application.h"
#include "AmberCraft/World.h"

RenderEngine::Systems::Application::Application()
{
	m_renderingManager = std::make_unique<AmberEngine::Managers::RenderingManager>();
}

void RenderEngine::Systems::Application::Setup()
{
	AmberEngine::Resources::Shader& chunkShader = m_renderingManager->GetResourcesManager()->LoadShaderFiles("chunk", "chunk.vs", "chunk.fs");
	AmberEngine::Resources::Texture& texture = m_renderingManager->GetResourcesManager()->LoadTexture("dirt", "blocks.png");
}

void RenderEngine::Systems::Application::Run()
{
	AmberCraft::World world;
	world.GenerateTerrain();
	world.Update();

	while (m_renderingManager->IsRunning())
	{
		m_renderingManager->Clear();
		m_renderingManager->Update();

		glm::vec3 playerPos = m_renderingManager->GetCamera()->GetPosition();
		std::array<uint64_t, 3> playerRoundedPos;
		playerRoundedPos[0] = glm::round(playerPos.x);
		playerRoundedPos[1] = glm::round(playerPos.y);
		playerRoundedPos[2] = glm::round(playerPos.z);

		ImGui::Begin("Scene");
		ImGui::Text("Camera Position X: %.1f Y: %.1f Z: %.1f", playerPos.x, playerPos.y, playerPos.z);
		//ImGui::Text("Overlapped Block %.1u", world.GetBlock(playerRoundedPos[0], playerRoundedPos[1], playerRoundedPos[2]));
		ImGui::End();

		world.Draw(*m_renderingManager);

		m_renderingManager->SwapBuffers();
	}
}