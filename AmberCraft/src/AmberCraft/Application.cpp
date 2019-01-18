#include "pch.h"

#include "AmberEngine/ImGui/imgui.h"

#include "AmberCraft/Application.h"
#include "AmberCraft/Blocktype.h"

RenderEngine::Systems::Application::Application() : disableShadows(false)
{
	m_shader = m_renderingManager.GetResourcesManager().LoadShaderFiles("chunk", "chunk.vs", "chunk.fs");
	m_shader.Bind();
	m_shader.SetUniform1i("disableShadows", disableShadows);
	m_shader.Unbind();
}

void RenderEngine::Systems::Application::Setup()
{
	AmberEngine::Resources::Texture& texture = m_renderingManager.GetResourcesManager().LoadTexture("dirt", "blocks.png");
}

void RenderEngine::Systems::Application::Run()
{
	m_world.GenerateTerrain();
	m_world.Update();

	while (m_renderingManager.IsRunning())
	{
		m_renderingManager.Clear();
		m_renderingManager.Update();

		//Temporary Player controller
		glm::vec3 playerPosition = m_renderingManager.GetCamera().GetPosition();
		glm::vec3 playerForward = m_renderingManager.GetCamera().GetForward();

		if (m_renderingManager.GetInputManager().IsKeyEventOccured(0x46))
		{
			disableShadows = !disableShadows;
			m_shader.Bind();
			m_shader.SetUniform1i("disableShadows", disableShadows);
		}

		//Break block
		/*if (m_renderingManager.GetInputManager().IsKeyEventOccured(0x52)) 
		{
			for (uint16_t currentStep = 0; currentStep < raycastSteps && !raycastHit; ++currentStep)
			{
				glm::vec3 raycastHitPoint = (playerPosition + playerForward * raycastDistance * (float)currentStep / raycastSteps + round);

				AmberCraft::BlockType blockFound = m_world.GetBlock(raycastHitPoint.x, raycastHitPoint.y, raycastHitPoint.z).type;

				if (blockFound != AmberCraft::BlockType::AIR)
				{
					raycastHit = m_world.SetBlock(raycastHitPoint.x, raycastHitPoint.y, raycastHitPoint.z, AmberCraft::BlockData{ AmberCraft::BlockType::AIR }, true);
				}
			}
		}*/

		if (m_renderingManager.GetInputManager().IsKeyEventOccured(0x52))
		{
			float raycastDistance = 10;
			
			RaycastCollision result;

			RayCast(playerPosition, playerForward, raycastDistance, result);

			if(result.type != AmberCraft::BlockType::AIR)
			{
				m_world.SetBlock(result.collisionPoint.x, result.collisionPoint.y, result.collisionPoint.z, AmberCraft::BlockData{ AmberCraft::BlockType::AIR }, true);
			}
		}

		//Add block
		/*if (m_renderingManager.GetInputManager().IsKeyEventOccured(0x53))
		{
			for (uint16_t currentStep = 0; currentStep < raycastSteps && !raycastHit; ++currentStep)
			{
				glm::vec3 raycastHitPoint = (playerPos + playerForward * raycastDistance * (float)currentStep / raycastSteps + round);

				AmberCraft::BlockType blockFound = m_world.GetBlock(raycastHitPoint.x, raycastHitPoint.y, raycastHitPoint.z).type;

				glm::vec3 normalHitPoint = playerPosition - raycastHitPoint;
				glm::normalize(normalHitPoint);
				glm::abs(normalHitPoint.x);

				std::max(std::max(glm::abs(normalHitPoint.x), glm::abs(normalHitPoint.y)), glm::abs(normalHitPoint.z));

				if (normalHitPoint.x > normalHitPoint.y && normalHitPoint.x > normalHitPoint.z)
					glm::vec3 newBlockPos(normalHitPoint.x, 0, 0);


				if (blockFound == AmberCraft::BlockType::AIR)
				{
					raycastHit = m_world.SetBlock(raycastHitPoint.x, raycastHitPoint.y, raycastHitPoint.z, AmberCraft::BlockData{ AmberCraft::BlockType::DIRT }, true);
				}
			}
		}*/
		//Temporary Player

		m_world.Draw(m_renderingManager);

		std::array<uint64_t, 3> playerRoundedPos;
		playerRoundedPos[0] = glm::round(playerPosition.x);
		playerRoundedPos[1] = glm::round(playerPosition.y);
		playerRoundedPos[2] = glm::round(playerPosition.z);

		ImGui::Begin("Scene");
		ImGui::Text("Camera Position X: %.1f Y: %.1f Z: %.1f", playerPosition.x, playerPosition.y, playerPosition.z);
		ImGui::Text("Overlapped Chunk [%.1i, %.1i]", static_cast<int>(playerPosition.x / 16), static_cast<int>(playerPosition.z / 16));
		ImGui::Text("Layer Level [%.1i]", static_cast<int>(playerPosition.y / 16));
		ImGui::Text("Overlapped Block %.1u", m_world.GetBlock(playerRoundedPos[0], playerRoundedPos[1], playerRoundedPos[2]));
		ImGui::End();

		m_renderingManager.SwapBuffers();
	}
}

void RenderEngine::Systems::Application::RayCast(glm::vec3 p_source, glm::vec3 p_direction, float p_maxDistance, RaycastCollision& p_out)
{
	RaycastCollision& result = p_out;

	result.isFound = false;
	result.type = AmberCraft::BlockType::AIR;
	result.collisionPoint = glm::vec3(0.0f);
	result.blockPosition = glm::vec3(0.0f);
	result.collisionFaceNormal = glm::vec3(0.0f);

	float raycastSteps = 1000.0f;
	glm::vec3 round(0.5f, 0.5f, 0.5f);

	for (uint16_t currentStep = 0; currentStep < raycastSteps && !result.isFound; ++currentStep)
	{
		glm::vec3 raycastHitPoint = (p_source + p_direction * p_maxDistance * (currentStep / raycastSteps) + round);

		result.type = m_world.GetBlock(raycastHitPoint.x, raycastHitPoint.y, raycastHitPoint.z).type;

		result.isFound = true;
		result.collisionPoint = raycastHitPoint;
		result.blockPosition.x = glm::round(raycastHitPoint.x);
		result.blockPosition.y = glm::round(raycastHitPoint.y);
		result.blockPosition.z = glm::round(raycastHitPoint.z);
		result.blockToCollisionPointDirection = glm::normalize(result.collisionPoint - result.blockPosition);


		/* Calculate face normal */
		/*glm::vec3 unsignedDirectionFromBlockToImpact = glm::vec3(std::abs(result.blockToCollisionPointDirection.x), std::abs(result.blockToCollisionPointDirection.y), std::abs(result.blockToCollisionPointDirection.z));
		if (unsignedDirectionFromBlockToImpact.x > unsignedDirectionFromBlockToImpact.y && unsignedDirectionFromBlockToImpact.x > unsignedDirectionFromBlockToImpact.z)
			result.collisionFaceNormal = result.blockToCollisionPointDirection.x > 0 ? glm::vec3(1.f, 0.f, 0.f) : glm::vec3(-1.f, 0.f, 0.f);
		else if (unsignedDirectionFromBlockToImpact.y > unsignedDirectionFromBlockToImpact.x && unsignedDirectionFromBlockToImpact.y > unsignedDirectionFromBlockToImpact.z)
			result.collisionFaceNormal = result.blockToCollisionPointDirection.y > 0 ? glm::vec3(0.f, 1.f, 0.f) : glm::vec3(0.f, -1.f, 0.f);
		else
			result.collisionFaceNormal = result.blockToCollisionPointDirection.z > 0 ? glm::vec3(0.f, 0.f, 1.f) : glm::vec3(0.f, 0.f, -1.f);*/
	}
}
