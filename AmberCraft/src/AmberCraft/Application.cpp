#include "pch.h"

#include <AmberEngine/ImGUI/imgui.h>

#include "AmberCraft/Application.h"
#include "AmberCraft/Chunk.h"

RenderEngine::Systems::Application::Application() : disableShadows(false)
{
	m_shader = m_renderingManager.GetResourcesManager().LoadShaderFiles("chunk", "chunk.vs", "chunk.fs");
	m_shader.Bind();
	m_shader.SetUniform1i("disableShadows", disableShadows);
	m_shader.SetUniformVec3("skyColour", glm::vec3(0.5, 0.5, 0.5));
	m_shader.Unbind();
}

void RenderEngine::Systems::Application::Setup()
{
	AmberEngine::Resources::Texture& texture = m_renderingManager.GetResourcesManager().LoadTexture("atlasBlocks", "blocks.png");
}

void RenderEngine::Systems::Application::Run()
{
	m_world.GenerateTerrain();

	m_renderingManager.GetCamera().SetPosition(0.0f, 0.0f, 0.0f);

	AmberCraft::BlockType blocks[4] = { AmberCraft::BlockType::GRASS, AmberCraft::BlockType::DIRT, AmberCraft::BlockType::ROCK, AmberCraft::BlockType::BRICK };

	float spacing = ImGui::GetStyle().ItemInnerSpacing.x;

	glm::ivec3 currentChunkPosition = glm::ivec3(0);

	uint8_t counter = 0;

	while (m_renderingManager.IsRunning())
	{
		m_renderingManager.Clear();
		m_renderingManager.Update();

		//Temporary Player controller
		glm::vec3 playerPosition = m_renderingManager.GetCamera().GetPosition();
		glm::vec3 playerForward  = m_renderingManager.GetCamera().GetForward();

		std::array<uint64_t, 3> playerRoundedPos;
		playerRoundedPos[0] = glm::round(playerPosition.x);
		playerRoundedPos[1] = glm::round(playerPosition.y);
		playerRoundedPos[2] = glm::round(playerPosition.z);

		AmberCraft::Chunk* currentChunk = m_world.GetChunk(playerRoundedPos[0], playerRoundedPos[1], playerRoundedPos[2]);

		currentChunkPosition = glm::ivec3(0, 0, 0);
		if (currentChunk != nullptr)
		{
			currentChunkPosition = currentChunk->GetChunkCoordinatePosition();
		}

		m_world.UpdateChuncksFromPlayerPosition(playerPosition);

		auto pos = AmberCraft::World::PositionToChunkCoordinate(playerRoundedPos[0], playerRoundedPos[1], playerRoundedPos[2]);

		int posX = playerRoundedPos[0] / CHUNK_SIZE + WORLD_SIZE / 2;
		int posZ = playerRoundedPos[2] / CHUNK_SIZE + WORLD_SIZE / 2;

		int distance = glm::distance(glm::vec2(posX, posZ), glm::vec2(m_world.GetChunks()[0]->GetChunkCoordinatePosition().x, m_world.GetChunks()[0]->GetChunkCoordinatePosition().z));

		if(m_renderingManager.GetInputManager().IsKeyEventOccured('B'))
		{
			auto pos = AmberCraft::World::From1Dto3D(1);
			auto chunk = new AmberCraft::Chunk({ pos.at(0) , pos.at(1) , pos.at(2) });

			chunk->Fill(AmberCraft::BlockType::BRICK);
			m_world.AddChunck(chunk);

			m_world.SetNeighbors();
		}



		//ImGUI
		ImGui::Begin("Scene");
		ImGui::Text("Camera Position X: %.1f Y: %.1f Z: %.1f", playerPosition.x, playerPosition.y, playerPosition.z);
		ImGui::Text("Overlapped Block %.1u", m_world.GetBlock(playerRoundedPos[0], playerRoundedPos[1], playerRoundedPos[2]));
		ImGui::Text("Chunk Position [%.1i, %.1i]", currentChunkPosition.x, currentChunkPosition.z);
		ImGui::Text("Player Chunk Position [%.1i, %.1i]", posX, posZ);
		ImGui::Text("Player Chunk Position from world [%.1i, %.1i, %.1i]", pos[0], pos[1], pos[2]);
		ImGui::Text("DISTANCE [%.1i]", distance);
		ImGui::Text("Layer Level [%.1i]", currentChunkPosition.y);

		ImGui::Text("Block Selection");
		if (ImGui::ArrowButton("##left", ImGuiDir_Left))
		{
			if (counter <= 0)
			{
				counter = 0;
			}
			else
			{
				counter--;
			}
		}
		ImGui::SameLine();
		ImGui::Text("Block Type %d ", counter);
		ImGui::SameLine(0.0f, spacing);
		if (ImGui::ArrowButton("##right", ImGuiDir_Right))
		{
			counter = (counter + 1) % 4;
		}
		ImGui::SameLine();
		ImGui::End();

		if (m_renderingManager.GetInputManager().IsKeyEventOccured(0x46))
		{
			disableShadows = !disableShadows;
			m_shader.Bind();
			m_shader.SetUniform1i("disableShadows", disableShadows);
		}

		//BREAK
		if (m_renderingManager.GetInputManager().IsKeyPressed(0x1))
		{
			float raycastDistance = 10;
			
			RaycastCollision result;

			RayCast(playerPosition, playerForward, raycastDistance, result);

			if(result.isFound)
			{
				m_world.SetBlock(result.blockPosition.x, result.blockPosition.y, result.blockPosition.z, AmberCraft::BlockData{ AmberCraft::BlockType::AIR }, true);
			}
		}
		//ADD
		if (m_renderingManager.GetInputManager().IsKeyEventOccured(0x2))
		{
			float raycastDistance = 10;

			RaycastCollision result;

			RayCast(playerPosition, playerForward, raycastDistance, result);

			if (result.isFound)
			{
				m_world.SetBlock(result.blockPosition.x + result.collisionFaceNormal.x, result.blockPosition.y + result.collisionFaceNormal.y, result.blockPosition.z + result.collisionFaceNormal.z, AmberCraft::BlockData{ blocks[counter] }, true);
			}
		}

		m_world.Draw(m_renderingManager);

		m_renderingManager.SwapBuffers();
	}
}

void RenderEngine::Systems::Application::RayCast(glm::vec3 p_source, glm::vec3 p_direction, float p_maxDistance, RaycastCollision& p_out) const
{
	RaycastCollision& result = p_out;

	result.isFound = false;
	result.type = AmberCraft::BlockType::AIR;
	result.collisionPoint = glm::vec3(0.0f);
	result.blockPosition = glm::vec3(0.0f);
	result.collisionFaceNormal = glm::vec3(0.0f);

	float raycastSteps = 1000.0f;
	glm::vec3 round(0.5f, 0.5f, 0.5f);

	for (int currentStep = 0; currentStep < raycastSteps && !result.isFound; currentStep++)
	{
		glm::vec3 raycastHitPoint = p_source + p_direction * p_maxDistance * (currentStep / raycastSteps);
		glm::vec3 raycastBlockPosition = glm::round(raycastHitPoint);

		result.type = m_world.GetBlock(raycastBlockPosition.x, raycastBlockPosition.y, raycastBlockPosition.z).type;

		if (result.type != AmberCraft::BlockType::AIR)
		{
			result.isFound = true;
			result.collisionPoint = raycastHitPoint;
			result.blockPosition.x = raycastBlockPosition.x;
			result.blockPosition.y = raycastBlockPosition.y;
			result.blockPosition.z = raycastBlockPosition.z;

			result.blockToCollisionPointDirection = glm::normalize(result.collisionPoint - result.blockPosition);

			/* Calculate face normal */
			glm::vec3 unsignedDirectionFromBlockToImpact = glm::vec3(std::abs(result.blockToCollisionPointDirection.x), std::abs(result.blockToCollisionPointDirection.y), std::abs(result.blockToCollisionPointDirection.z));
			if (unsignedDirectionFromBlockToImpact.x > unsignedDirectionFromBlockToImpact.y && unsignedDirectionFromBlockToImpact.x > unsignedDirectionFromBlockToImpact.z)
				result.collisionFaceNormal = result.blockToCollisionPointDirection.x > 0 ? glm::vec3(1.f, 0.f, 0.f) : glm::vec3(-1.f, 0.f, 0.f);
			else if (unsignedDirectionFromBlockToImpact.y > unsignedDirectionFromBlockToImpact.x && unsignedDirectionFromBlockToImpact.y > unsignedDirectionFromBlockToImpact.z)
				result.collisionFaceNormal = result.blockToCollisionPointDirection.y > 0 ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(0.f, -1.f, 0.f);
			else
				result.collisionFaceNormal = result.blockToCollisionPointDirection.z > 0 ? glm::vec3(0.f, 0.f, 1.f) : glm::vec3(0.f, 0.f, -1.f);
		}
	}
}
