#include "AmberCraftPCH.h"

#include "AmberCraft/UI/DebugUI.h"

#include "AmberCraft/Terrain/World.h"
#include "AmberCraft/Gameplay/Player.h"

AmberCraft::UI::DebugUI::DebugUI(Terrain::World& p_world, Gameplay::Player& p_player) : m_world(p_world),
	m_player(p_player), m_title("DEBUG UI"), m_currenBlockType(Terrain::EBlockType::ROCK)
{
}

void AmberCraft::UI::DebugUI::Draw()
{
	const glm::vec3 playerPosition = m_player.GetPosition();
	
	ImGui::Begin(m_title.c_str());

	ImGui::Text("Camera Position X: %.1f Y: %.1f Z: %.1f", playerPosition.x, playerPosition.y, playerPosition.z);


	auto blockType = m_world.GetBlock(playerPosition.x, playerPosition.y, playerPosition.z).type;

	switch (blockType)
	{
	case Terrain::EBlockType::AIR:   m_currentBlockType = "Air";
		break;
	case Terrain::EBlockType::DIRT:   m_currentBlockType = "Dirt";
		break;
	case Terrain::EBlockType::GRASS: m_currentBlockType = "Grass";
		break;
	case Terrain::EBlockType::ROCK: m_currentBlockType = "Rock";
		break;
	case Terrain::EBlockType::BRICK: m_currentBlockType = "Brick";
		break;

		default:
			m_currentBlockType = "None";
			break;
	}

	auto [xt, yt, zt] = Terrain::World::GetChunkCoordinates(playerPosition);

	ImGui::Text("CURRENT CHUNK POSI test  [%.1i, %.1i, %.1i]", xt, yt, zt);

	ImGui::Text("Overlapped Block %s", m_currentBlockType);

	ImGui::Text("Layer Level [%.1i]", yt);

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

	ImGui::Checkbox("WORLD INFINITE GENERATION", &Terrain::World::__WORLD_IFINITE_GENERATION);
	
	if(ImGui::Checkbox("CHUNK SURFACE CULLING", &Terrain::Chunk::__CHUNK_SURFACE_CULLING) 
		|| ImGui::Checkbox("BLOCK SURFACE CULLING", &Terrain::Chunk::__BLOCK_SURFACE_CULLING))
	{
		m_world.UpdateChunksToRender();
	}

	ImGui::End();

	m_player.SetCurrentBlock(m_currenBlockType);
}
