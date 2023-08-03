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

	
	float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

	glm::ivec3 currentChunkPosition = glm::ivec3(0);

	int counter = 0;

	while (m_renderingManager.IsRunning())
	{
		m_renderingManager.Clear();
		m_renderingManager.Update();

		//Temporary Player controller
		glm::vec3 playerPosition = m_renderingManager.GetCamera().GetPosition();
		glm::vec3 playerForward  = m_renderingManager.GetCamera().GetForward();

		m_world->UpdateChuncksFromPlayerPosition(playerPosition);

		std::array<uint64_t, 3> playerRoundedPos;
		playerRoundedPos[0] = glm::round(playerPosition.x);
		playerRoundedPos[1] = glm::round(playerPosition.y);
		playerRoundedPos[2] = glm::round(playerPosition.z);

		Terrain::Chunk* currentChunk = m_world->GetChunk(playerRoundedPos[0], playerRoundedPos[1], playerRoundedPos[2]);

		currentChunkPosition = glm::ivec3(0, 0, 0);
		if (currentChunk != nullptr)
		{
			currentChunkPosition = currentChunk->GetChunkCoordinatePosition();
		}

		auto pos = Terrain::World::PositionToChunkCoordinate(playerRoundedPos[0], playerRoundedPos[1], playerRoundedPos[2]);

		int posX = playerRoundedPos[0] / CHUNK_SIZE + WORLD_SIZE / 2;
		int posZ = playerRoundedPos[2] / CHUNK_SIZE + WORLD_SIZE / 2;

		int distance = glm::distance(glm::vec2(posX, posZ), glm::vec2(m_world->GetChunks()[0]->GetChunkCoordinatePosition().x, m_world->GetChunks()[0]->GetChunkCoordinatePosition().z));

		if(m_renderingManager.GetInputManager().IsKeyEventOccured('B'))
		{
			auto pos = Terrain::World::From1Dto3D(1);
			auto chunk = new Terrain::Chunk({ pos.at(0) , pos.at(1) , pos.at(2) });

			chunk->Fill(Terrain::EBlockType::BRICK);
			m_world->AddChunck(chunk);

			m_world->SetNeighbors();
		}

		//ImGUI
		ImGui::Begin("Scene");
		ImGui::Text("Camera Position X: %.1f Y: %.1f Z: %.1f", playerPosition.x, playerPosition.y, playerPosition.z);
		ImGui::Text("Overlapped Block %.1u", m_world->GetBlock(playerRoundedPos[0], playerRoundedPos[1], playerRoundedPos[2]));
		ImGui::Text("Chunk Position [%.1i, %.1i]", currentChunkPosition.x, currentChunkPosition.z);
		ImGui::Text("Player Chunk Position [%.1i, %.1i]", posX, posZ);
		ImGui::Text("Player Chunk Position from world [%.1i, %.1i, %.1i]", pos[0], pos[1], pos[2]);
		ImGui::Text("DISTANCE [%.1i]", distance);
		ImGui::Text("Layer Level [%.1i]", currentChunkPosition.y);

		ImGui::Text("Block Selection");
		if (ImGui::ArrowButton("##left", ImGuiDir_Left))
		{
			counter = (counter - 1) % 5;
		}
		ImGui::SameLine();
		ImGui::Text("Block Type %d ", std::abs(counter));
		ImGui::SameLine(0.0f, spacing);
		if (ImGui::ArrowButton("##right", ImGuiDir_Right))
		{
			counter = (counter + 1) % 5;
		}
		ImGui::SameLine();
		ImGui::End();

		m_player->SetCurrentBlock(static_cast<Terrain::EBlockType>(std::abs(counter)));

		if (m_renderingManager.GetInputManager().IsKeyEventOccured(0x46))
		{
			disableShadows = !disableShadows;
			m_shader.Bind();
			m_shader.SetUniform1i("disableShadows", disableShadows);
		}

		m_player->Update();

		m_world->Draw(m_renderingManager);

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