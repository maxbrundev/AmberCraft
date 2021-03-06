#include "pch.h"

#include "AmberCraft/World.h"
#include "AmberCraft/FastNoise.h"

AmberCraft::World::World() : m_chunks(WORLD_ELEMENTS_COUNT)
{
	SetNeighbors();
}

void AmberCraft::World::SetNeighbors()
{
	for (uint16_t i = 0; i < WORLD_ELEMENTS_COUNT; ++i)
	{
		auto [x, y, z] = From1Dto3D(i);

		Chunk* left	 = IsInWorld(x - 1)	? &m_chunks[From3Dto1D(x - 1, y, z)] : nullptr;
		Chunk* right = IsInWorld(x + 1)	? &m_chunks[From3Dto1D(x + 1, y, z)] : nullptr;
		Chunk* bot	 = IsInWorld(y - 1)	? &m_chunks[From3Dto1D(x, y - 1, z)] : nullptr;
		Chunk* top	 = IsInWorld(y + 1) ? &m_chunks[From3Dto1D(x, y + 1, z)] : nullptr;
		Chunk* back	 = IsInWorld(z - 1)	? &m_chunks[From3Dto1D(x, y, z - 1)] : nullptr;
		Chunk* front = IsInWorld(z + 1)	? &m_chunks[From3Dto1D(x, y, z + 1)] : nullptr;

		m_chunks[i].SetChunksNeighbors(left, right, top, bot, front, back);
	}
}

bool AmberCraft::World::IsInWorld(uint8_t p_index)
{
	return p_index >= 0 && p_index <= WORLD_SIZE - 1;
}

void AmberCraft::World::GenerateTerrain()
{
	for (uint16_t i = 0; i < WORLD_ELEMENTS_COUNT; ++i)
	{
		m_chunks[i].FillChunk(BlockType::AIR);
	}

	FastNoise perlinNoise;
	perlinNoise.SetSeed(747);

	uint64_t surfaceLength = WORLD_SIZE * CHUNK_SIZE;
	const float frequency = 5.0f; // Higher frequency implies abrupt terrain
	const float amplitude = 20;
	const float minimumHeight = 64;

	for (uint64_t z = 0; z < surfaceLength; ++z)
	{
		for (uint64_t x = 0; x < surfaceLength; ++x)
		{
			float perlinValue = (((perlinNoise.GetPerlin(x * frequency, z * frequency) + 1.0f) / 2.0f) * amplitude) + minimumHeight - amplitude * 0.5f;

			uint8_t layer = 0;

			for(int16_t currentHeight = static_cast<int16_t>(perlinValue); currentHeight >= 0; --currentHeight)
			{
				BlockData newBlock;

				if (layer == 0)
					newBlock.type = BlockType::GRASS;
				else if (layer <= 2)
					newBlock.type = BlockType::DIRT;
				else
					newBlock.type = BlockType::ROCK;

				SetBlock(static_cast<uint64_t>(x) - (WORLD_SIZE / 2) * CHUNK_SIZE, static_cast<uint64_t>(currentHeight), static_cast<uint64_t>(z) - (WORLD_SIZE / 2) * CHUNK_SIZE, newBlock);
				++layer;
			}
		}
	}
}

void AmberCraft::World::Update()
{
	for (uint16_t i = 0; i < WORLD_ELEMENTS_COUNT; ++i)
	{
		m_chunks[i].Update();
	}
}

void AmberCraft::World::Draw(AmberEngine::Managers::RenderingManager& p_renderingManager)
{
	glm::mat4 projectionMatrix = p_renderingManager.CalculateProjectionMatrix();
	glm::mat4 viewMatrix = p_renderingManager.CalculateViewMatrix();

	auto& chunkShader = p_renderingManager.GetResourcesManager().GetShader("chunk");

	p_renderingManager.GetResourcesManager().GetTexture("atlasBlocks").Bind();

	chunkShader.Bind();
	chunkShader.SetUniformMat4("projection", projectionMatrix);
	chunkShader.SetUniformMat4("view", viewMatrix);
	chunkShader.SetUniform1i("atlas", 0);
	
	for (uint16_t i = 0; i < WORLD_ELEMENTS_COUNT; ++i)
	{
		auto [x, y, z] = From1Dto3D(i);

		glm::vec3 chunkPosition((x - WORLD_SIZE / 2) * CHUNK_SIZE, y * CHUNK_SIZE, (z - WORLD_SIZE / 2) * CHUNK_SIZE);

		if(glm::distance(chunkPosition, p_renderingManager.GetCamera().GetPosition()) <= 200)
		{
			chunkShader.SetUniformMat4("model", glm::translate(glm::mat4(1.0f), chunkPosition));
			m_chunks[i].Draw();
		}
	}
}

AmberCraft::BlockData AmberCraft::World::GetBlock(uint64_t p_x, uint64_t p_y, uint64_t p_z)
{
	uint16_t chunkElement = From3Dto1D((p_x / CHUNK_SIZE) + WORLD_SIZE / 2, p_y / CHUNK_SIZE, (p_z / CHUNK_SIZE) + WORLD_SIZE / 2);
	uint16_t blockElement = Chunk::From3Dto1D((p_x + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE, p_y % CHUNK_SIZE, (p_z + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE);

	if (chunkElement >= WORLD_ELEMENTS_COUNT || blockElement >= CHUNK_ELEMENTS_COUNT)
		return BlockData{BlockType::AIR};

	return m_chunks[chunkElement].blocks[blockElement];
}

bool AmberCraft::World::SetBlock(uint64_t p_x, uint64_t p_y, uint64_t p_z, BlockData p_blockData, bool p_updateChunk)
{
	uint16_t chunkElement = From3Dto1D((p_x / CHUNK_SIZE) + WORLD_SIZE / 2, p_y / CHUNK_SIZE, (p_z / CHUNK_SIZE) + WORLD_SIZE / 2);
	uint16_t blockElement = Chunk::From3Dto1D((p_x + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE, p_y % CHUNK_SIZE, (p_z + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE);

	if (chunkElement >= WORLD_ELEMENTS_COUNT || blockElement >= CHUNK_ELEMENTS_COUNT)
		return false;

	m_chunks[chunkElement].blocks[blockElement] = p_blockData;

	if (p_updateChunk)
	{
		m_chunks[chunkElement].Update();
		m_chunks[chunkElement].UpdateNeighBors();
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