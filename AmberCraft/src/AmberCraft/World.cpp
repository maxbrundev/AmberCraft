#include "pch.h"

#include "AmberCraft/World.h"

#include "AmberCraft/FastNoise.h"

AmberCraft::World::World()
{
	for (int i = 0; i < WORLD_ELEMENTS_COUNT; i++)
	{
		std::array<uint8_t, 3> position = From1Dto3D(i);

		m_chunks.push_back(new Chunk({ position.at(0), position.at(1), position.at(2) }));
	}

	SetNeighbors();
}

AmberCraft::World::~World()
{
	for (auto& chunk : m_chunks)
	{
		delete chunk;
		chunk = nullptr;
	}

	m_chunks.clear();
}

void AmberCraft::World::UpdateChuncksFromPlayerPosition(const glm::vec3& p_playerPosition)
{
	//RemoveChunckAwayFromPlayer(p_playerPosition);

	glm::vec2 distancePlayer = glm::vec2(p_playerPosition.x, p_playerPosition.z) - glm::vec2(m_offsetX * CHUNK_SIZE, m_offsetZ * CHUNK_SIZE);

	if (length(distancePlayer) < CHUNK_SIZE * 1.5f)
		return;

	distancePlayer = normalize(distancePlayer);

	SetShiftWorldDirection(distancePlayer);

	std::array<uint64_t, 3> playerRoundedPos;

	playerRoundedPos[0] = std::round(p_playerPosition.x);
	playerRoundedPos[1] = std::round(p_playerPosition.y);
	playerRoundedPos[2] = std::round(p_playerPosition.z);

	for (int i = 0; i < m_chunks.size(); i++)
	{
		int posX = playerRoundedPos[0] / CHUNK_SIZE + WORLD_SIZE / 2;
		int posZ = playerRoundedPos[2] / CHUNK_SIZE + WORLD_SIZE / 2;

		int distance = glm::distance(glm::vec2(posX, posZ), glm::vec2(m_chunks[i]->GetChunkCoordinatePosition().x, m_chunks[i]->GetChunkCoordinatePosition().z));

		glm::vec2 distance2 = glm::vec2(posX, posZ) - glm::vec2(m_chunks[i]->GetChunkCoordinatePosition().x, m_chunks[i]->GetChunkCoordinatePosition().z);

		if(distance > WORLD_SIZE / 2)
		{
			glm::ivec3 newChunkPos = glm::ivec3(0);

			distance2 = normalize(distance2);
	
			if (distance2.x > 0.5f)
			{
				newChunkPos.x += WORLD_SIZE;
			}
			else if (distance2.x < -0.5f)
			{
				newChunkPos.x += -WORLD_SIZE;
			}

			if (distance2.y > 0.5f)
			{
				newChunkPos.z += WORLD_SIZE;
			}
			else if (distance2.y < -0.5f)
			{
				newChunkPos.z += -WORLD_SIZE;
			}
			
			m_chunks[i]->SetPosition(m_chunks[i]->GetChunkCoordinatePosition() + newChunkPos);
		}
	}
}

void AmberCraft::World::RemoveChunckAwayFromPlayer(const glm::vec3& p_playerPosition)
{
	const glm::vec2 distancePlayer = glm::vec2(p_playerPosition.x, p_playerPosition.z) - glm::vec2(
		m_offsetX * CHUNK_SIZE, m_offsetZ * CHUNK_SIZE);
	
	if (length(distancePlayer) < CHUNK_SIZE * 1.5)
		return;
	
	SetShiftWorldDirection(normalize(distancePlayer));
}

void AmberCraft::World::SetNeighbors()
{
	for (int i = 0; i < m_chunks.size(); i++)
	{
		auto [x, y, z] = From1Dto3D(i);

		Chunk* left  = IsInWorld(x - 1) ? m_chunks[From3Dto1D(x - 1, y, z)] : nullptr;
		Chunk* right = IsInWorld(x + 1) ? m_chunks[From3Dto1D(x + 1, y, z)] : nullptr;
		Chunk* bot   = IsInWorld(y - 1) ? m_chunks[From3Dto1D(x, y - 1, z)] : nullptr;
		Chunk* top   = IsInWorld(y + 1) ? m_chunks[From3Dto1D(x, y + 1, z)] : nullptr;
		Chunk* back  = IsInWorld(z - 1) ? m_chunks[From3Dto1D(x, y, z - 1)] : nullptr;
		Chunk* front = IsInWorld(z + 1) ? m_chunks[From3Dto1D(x, y, z + 1)] : nullptr;

		m_chunks[i]->SetChunksNeighbors(left, right, top, bot, front, back);
	}
}

bool AmberCraft::World::IsInWorld(uint8_t p_index)
{
	return p_index >= 0 && p_index <= WORLD_SIZE - 1;
}

void AmberCraft::World::AddChunck(Chunk* p_chunck)
{
	m_chunks.push_back(p_chunck);
}

void AmberCraft::World::GenerateTerrain() const
{
	FastNoise perlinNoise;
	perlinNoise.SetSeed(747);

	uint64_t surfaceLength = WORLD_SIZE * CHUNK_SIZE;
	const float frequency = 5.0f; // Higher frequency implies abrupt terrain
	const float amplitude = 20;
	const float minimumHeight = 64;

	for (uint64_t z = 0; z < surfaceLength; z++)
	{
		for (uint64_t x = 0; x < surfaceLength; x++)
		{
			const float perlinValue = (((perlinNoise.GetPerlin(x * frequency, z * frequency) + 1.0f) / 2.0f) * amplitude) + minimumHeight - amplitude * 0.5f;

			uint8_t layer = 0;

			for(int16_t currentHeight = static_cast<int16_t>(perlinValue); currentHeight >= 0; currentHeight--)
			{
				BlockData newBlock;

				if (layer == 0)
					newBlock.type = BlockType::GRASS;
				else if (layer <= 2)
					newBlock.type = BlockType::DIRT;
				else
					newBlock.type = BlockType::ROCK;

				SetBlock(static_cast<uint64_t>(x) - (WORLD_SIZE / 2) * CHUNK_SIZE, static_cast<uint64_t>(currentHeight) - (WORLD_SIZE / 2) * CHUNK_SIZE, static_cast<uint64_t>(z) - (WORLD_SIZE / 2) * CHUNK_SIZE, newBlock);

				++layer;
			}
		}
	}

	UpdateChuncksToRender();
}

void AmberCraft::World::SetShiftWorldDirection(const glm::vec2& p_direction)
{
	if (p_direction.x > 0.33f)
	{
		Shift(EShiftDirection::EST);
	}
	else if (p_direction.x < -0.33f)
	{
		Shift(EShiftDirection::WEST);
	}

	if (p_direction.y > 0.33f)
	{
		Shift(EShiftDirection::SOUTH);
	}
	else if (p_direction.y < -0.33f)
	{
		Shift(EShiftDirection::NORTH);
	}
}

void AmberCraft::World::Shift(EShiftDirection p_direction)
{
	switch (p_direction)
	{
	case EShiftDirection::NORTH:
		m_offsetZ--;
		break;
	case EShiftDirection::SOUTH:
		m_offsetZ++;
		break;
	case EShiftDirection::EST:
		m_offsetX++;
		break;
	case EShiftDirection::WEST:
		m_offsetX--;
		break;
	}
}

void AmberCraft::World::UpdateChuncksToRender() const
{
	for (int i = 0; i < m_chunks.size(); ++i)
	{
		m_chunks[i]->UpdateBlocksToRender();
	}
}

void AmberCraft::World::Draw(AmberEngine::Managers::RenderingManager& p_renderingManager) const
{
	const glm::mat4 projectionMatrix = p_renderingManager.CalculateProjectionMatrix();
	const glm::mat4 viewMatrix       = p_renderingManager.CalculateViewMatrix();

	auto& chunkShader = p_renderingManager.GetResourcesManager().GetShader("chunk");

	p_renderingManager.GetResourcesManager().GetTexture("atlasBlocks").Bind();

	chunkShader.Bind();
	chunkShader.SetUniformMat4("projection", projectionMatrix);
	chunkShader.SetUniformMat4("view", viewMatrix);
	chunkShader.SetUniform1i("atlas", 0);
	
	for (int i = 0; i < m_chunks.size(); i++)
	{
		const glm::vec3 chunkPosition = m_chunks[i]->GetWorldCoordinatePosition();

		chunkShader.SetUniformMat4("model", glm::translate(glm::mat4(1.0f), chunkPosition));
		m_chunks[i]->Draw();
	}
}

AmberCraft::Chunk* AmberCraft::World::GetChunk(uint64_t p_x, uint64_t p_y, uint64_t p_z) const
{
	const uint16_t chunkElement = From3Dto1D((p_x / CHUNK_SIZE) + WORLD_SIZE / 2, p_y / CHUNK_SIZE, (p_z / CHUNK_SIZE) + WORLD_SIZE / 2);

	if (chunkElement >= m_chunks.size())
		return nullptr;

	return m_chunks[chunkElement];
}

AmberCraft::BlockData AmberCraft::World::GetBlock(uint64_t p_x, uint64_t p_y, uint64_t p_z) const
{
	const uint16_t chunkElement = From3Dto1D(p_x / CHUNK_SIZE + WORLD_SIZE / 2, p_y / CHUNK_SIZE + WORLD_SIZE / 2, p_z / CHUNK_SIZE + WORLD_SIZE / 2);
	const uint16_t blockElement = Chunk::From3Dto1D((p_x + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE, (p_y + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE, (p_z + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE);

	if (chunkElement >= m_chunks.size() || blockElement >= CHUNK_ELEMENTS_COUNT)
		return BlockData{BlockType::AIR};

	return m_chunks[chunkElement]->blocks[blockElement];
}

bool AmberCraft::World::SetBlock(uint64_t p_x, uint64_t p_y, uint64_t p_z, BlockData p_blockData, bool p_updateChunk) const
{
	const uint16_t chunkElement = From3Dto1D(p_x / CHUNK_SIZE + WORLD_SIZE / 2, p_y / CHUNK_SIZE + WORLD_SIZE / 2, p_z / CHUNK_SIZE + WORLD_SIZE / 2);
	const uint16_t blockElement = Chunk::From3Dto1D((p_x + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE, (p_y + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE, (p_z + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE);

	if (chunkElement >= m_chunks.size() || blockElement >= CHUNK_ELEMENTS_COUNT)
		return false;

	m_chunks[chunkElement]->blocks[blockElement] = p_blockData;

	if (p_updateChunk)
	{
		m_chunks[chunkElement]->UpdateBlocksToRender();
		m_chunks[chunkElement]->UpdateNeighBors();
	}

	return true;
}

std::array<uint8_t, 3> AmberCraft::World::From1Dto3D(uint16_t p_index)
{
	std::array<uint8_t, 3> result;

	result[2] = p_index / (WORLD_SIZE * WORLD_SIZE);
	p_index -= (std::get<2>(result) * (WORLD_SIZE * WORLD_SIZE));

	result[1] = p_index / WORLD_SIZE;
	result[0] = p_index % WORLD_SIZE;

	return result;
}

uint16_t AmberCraft::World::From3Dto1D(uint8_t p_x, uint8_t p_y, uint8_t p_z)
{
	return p_x + p_y * WORLD_SIZE + p_z * WORLD_SIZE * WORLD_SIZE;
}

std::array<uint64_t, 3> AmberCraft::World::PositionToChunkCoordinate(uint64_t p_x, uint64_t p_y, uint64_t p_z)
{
	std::array<uint64_t, 3> result;

	result[0] = p_x / CHUNK_SIZE + WORLD_SIZE / 2;
	result[1] = p_y / CHUNK_SIZE + WORLD_SIZE / 2;
	result[2] = p_z / CHUNK_SIZE + WORLD_SIZE / 2;

	return result;
}
