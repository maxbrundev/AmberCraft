#include "AmberCraftPCH.h"

#include "AmberCraft/UI/DebugUI.h"

#include "AmberCraft/Terrain/World.h"
#include "AmberCraft/Gameplay/Player.h"
#include "AmberCraft/Utils/WorldUtils.h"

AmberCraft::UI::DebugUI::DebugUI(Terrain::World& p_world, Gameplay::Player& p_player) : m_world(p_world),
	m_player(p_player), m_title("DEBUG UI"), m_currenBlockType(Terrain::EBlockType::ROCK)
{
}

void AmberCraft::UI::DebugUI::Draw()
{
	glm::vec3 playerPosition = m_player.GetPosition();

	ImGui::Begin(m_title.c_str());

	ImGui::Text("Camera Position X: %.1f Y: %.1f Z: %.1f", playerPosition.x, playerPosition.y, playerPosition.z);

	glm::ivec3 playerchunkPosition = Utils::WorldUtils::GetWorldToChunkCoordinate(playerPosition);

	ImGui::Text("Overlapped Block %.1u", m_world.GetBlock(playerchunkPosition.x, playerchunkPosition.y, playerchunkPosition.z));

	ImGui::Text("Player Chunk Position from world [%.1i, %.1i, %.1i]", playerchunkPosition.x, playerchunkPosition.y, playerchunkPosition.z);
	
	ImGui::Text("DISTANCE [%.1i]", Utils::WorldUtils::GetWorldDistanceInChunkCoordinate(playerPosition, m_world.GetChunks()[0]->GetWorldCoordinatePosition()));
	
	ImGui::Text("Layer Level [%.1i]", playerchunkPosition.y);

	ImGui::Text("Block Selection");
	if (ImGui::ArrowButton("##left", ImGuiDir_Left))
	{
		m_currenBlockType = static_cast<Terrain::EBlockType>((static_cast<uint8_t>(m_currenBlockType) - 1) % 5);
	}
	ImGui::SameLine();
	ImGui::Text("Block Type %i", m_currenBlockType);
	ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
	if (ImGui::ArrowButton("##right", ImGuiDir_Right))
	{
		m_currenBlockType = static_cast<Terrain::EBlockType>((static_cast<uint8_t>(m_currenBlockType) + 1) % 5);
	}
	ImGui::SameLine();
	ImGui::End();

	m_player.SetCurrentBlock(m_currenBlockType);
}
