#include "AmberCraftPCH.h"

#include "AmberCraft/Terrain/World.h"

#include "AmberCraft/Terrain/Chunk.h"

#include "AmberCraft/Noise/PerlinNoise.h"

bool AmberCraft::Terrain::World::__WORLD_IFINITE_GENERATION = false;

AmberCraft::Terrain::World::World()
{
	m_chunks.reserve(WORLD_ELEMENTS_COUNT);
	m_xChunkOffset = 0;
	m_zChunkOffset = 0;

	m_perlin = std::make_unique<Noise::PerlinNoise>(m_noiseData.Seed);

	m_terrainLayers.clear();
	m_terrainLayers.emplace_back("Grass", EBlockType::GRASS, 1);
	m_terrainLayers.emplace_back("Dirt", EBlockType::DIRT, 3);
	m_terrainLayers.emplace_back("Stone", EBlockType::ROCK, 255);

	m_fastNoise.SetSeed(m_noiseData.Seed);
	m_fastNoise.SetFractalType(FastNoise::FBM);
	m_fastNoise.SetFractalGain(1);
	m_fastNoise.SetFractalOctaves(6);
	m_fastNoise.SetFrequency(1.2f);
	m_fastNoise.SetFractalLacunarity(0.5f);

	for (size_t  i = 0; i < WORLD_ELEMENTS_COUNT; i++)
	{
		const auto [x, y, z] = From1Dto3D(i);

		m_chunks.push_back(new Chunk(glm::vec3(
			(x - WORLD_SIZE / 2) * CHUNK_SIZE,
			y * CHUNK_SIZE,
			(z - WORLD_SIZE / 2) * CHUNK_SIZE)));
	}

	SetNeighbors();
}

AmberCraft::Terrain::World::~World()
{
	for (auto& chunk : m_chunks)
	{
		delete chunk;
		chunk = nullptr;
	}

	m_chunks.clear();
}

void AmberCraft::Terrain::World::GenerateTerrain() const
{
	for (uint8_t z = 0; z < WORLD_SIZE; z++)
	{
		for (uint8_t x = 0; x < WORLD_SIZE; x++)
		{
			GenerateChunk(x, z);
		}
	}

	UpdateChunksToRender();
}

void AmberCraft::Terrain::World::GenerateChunk(uint8_t p_xChunk, uint8_t p_zChunk, bool p_update) const
{
	for (uint8_t zBlock = 0; zBlock < CHUNK_SIZE; zBlock++)
	{
		for (uint8_t xBlock = 0; xBlock < CHUNK_SIZE; xBlock++)
		{
			const int64_t localX = xBlock + p_xChunk * CHUNK_SIZE;
			const int64_t localZ = zBlock + p_zChunk * CHUNK_SIZE;

			const int64_t worldX = LocalToWorldX(localX);
			const int64_t worldZ = LocalToWorldZ(localZ);

			const int16_t surfaceHeight = HeightAt(static_cast<int16_t>(worldX), static_cast<int16_t>(worldZ));

			ApplyTerrainLayers(worldX, surfaceHeight, worldZ, p_update);
		}
	}
}

void AmberCraft::Terrain::World::ApplyTerrainLayers(int64_t p_worldX, int16_t p_height, int64_t p_worldZ, bool p_update) const
{
	int16_t currentHeight = p_height;

	for (const auto& terrainLayer : m_terrainLayers)
	{
		uint8_t counter = 0;

		while (counter != terrainLayer.Height && currentHeight >= 0)
		{
			SetBlock(p_worldX, currentHeight, p_worldZ, terrainLayer.BlockData, p_update);

			counter++;
			currentHeight--;
		}
	}
}

void AmberCraft::Terrain::World::SetNeighbors() const
{
	for (size_t i = 0; i < m_chunks.size(); i++)
	{
		const auto[x, y, z] = From1Dto3D(i);
		SetNeighbor(x, y, z);
	}
}

void AmberCraft::Terrain::World::SetNeighbor(uint8_t p_x, uint8_t p_y, uint8_t p_z) const
{
	const auto chunkComponent = m_chunks[From3Dto1D(p_x, p_y, p_z)];

	Chunk* left = p_x != 0 ?               m_chunks[From3Dto1D(p_x - 1, p_y, p_z)] : nullptr;
	Chunk* right = p_x != WORLD_SIZE - 1 ? m_chunks[From3Dto1D(p_x + 1, p_y, p_z)] : nullptr;
	Chunk* bot = p_y != 0 ?                m_chunks[From3Dto1D(p_x, p_y - 1, p_z)] : nullptr;
	Chunk* top = p_y != WORLD_SIZE - 1 ?   m_chunks[From3Dto1D(p_x, p_y + 1, p_z)] : nullptr;
	Chunk* back = p_z != 0 ?               m_chunks[From3Dto1D(p_x, p_y, p_z - 1)] : nullptr;
	Chunk* front = p_z != WORLD_SIZE - 1 ? m_chunks[From3Dto1D(p_x, p_y, p_z + 1)] : nullptr;

	chunkComponent->SetChunksNeighbors(left, right, top, bot, front, back);
}

void AmberCraft::Terrain::World::UpdateChunksFromPlayerPosition(const glm::vec3& p_playerPosition, const glm::vec3& p_playerForwad)
{
	if(__WORLD_IFINITE_GENERATION)
	{
		if (!m_shiftWorldQueue.empty())
		{
			ShiftWorld(m_shiftWorldQueue.front().Direction.first, m_shiftWorldQueue.front().Direction.second);
		}
		else
		{
			const glm::vec2 distancePlayer = glm::vec2(p_playerPosition.x, p_playerPosition.z) - glm::vec2(m_xChunkOffset * CHUNK_SIZE, m_zChunkOffset * CHUNK_SIZE);
	
			if (glm::length(distancePlayer) < CHUNK_SIZE * 1.5)
				return;

			SetShiftWorldDirection(glm::normalize(distancePlayer));
		}
	}
}

constexpr int64_t AmberCraft::Terrain::World::LocalToWorldX(int64_t p_localX) const
{
	return p_localX - (WORLD_SIZE / 2 - m_xChunkOffset) * CHUNK_SIZE;
}

constexpr int64_t AmberCraft::Terrain::World::LocalToWorldZ(int64_t p_localZ) const
{
	return p_localZ - (WORLD_SIZE / 2 - m_zChunkOffset) * CHUNK_SIZE;
}

uint8_t AmberCraft::Terrain::World::HeightAt(int16_t p_x, int16_t p_y) const
{
	float noiseHeight = 0.0f;
	float amplitude = 1.0f;
	float frequency = 1.0f;

	for (uint8_t i = 0; i < m_noiseData.Octaves; i++)
	{
		const float sampleX = p_x / m_noiseData.Scale * frequency;
		const float sampleY = p_y / m_noiseData.Scale * frequency;

		const float perlinValue = m_fastNoise.GetPerlinFractal(sampleX, sampleY, 0.2f);

		noiseHeight += perlinValue * amplitude;

		amplitude *= m_noiseData.Persistence;
		frequency *= m_noiseData.Lacunarity;
	}

	noiseHeight = m_noiseData.MaxHeight - floor(noiseHeight * m_noiseData.MaxHeight);

	noiseHeight = std::pow(noiseHeight * m_noiseData.HeightScale, m_noiseData.Redistribution);

	return static_cast<uint8_t>(noiseHeight + m_noiseData.AdditionalElevation);
}

void AmberCraft::Terrain::World::SetShiftWorldDirection(const glm::vec2& p_direction)
{
	if (p_direction.x > 0.33f)
	{
		m_shiftWorldQueue.emplace(std::make_pair(0, 1));
	}
	else if (p_direction.x < -0.33f)
	{
		m_shiftWorldQueue.emplace(std::make_pair(0, -1));
	}

	if (p_direction.y > 0.33f)
	{
		m_shiftWorldQueue.emplace(std::make_pair(2, 1));
	}
	else if (p_direction.y < -0.33f)
	{
		m_shiftWorldQueue.emplace(std::make_pair(2, -1));
	}
}

void AmberCraft::Terrain::World::ShiftWorld(uint8_t p_axis, int8_t p_shiftDirection)
{
	const uint8_t indiceChunksToDelete = p_shiftDirection > 0 ? 0 : WORLD_SIZE - 1;
	const uint8_t indiceChunksToAdd    = p_shiftDirection > 0 ? WORLD_SIZE - 1 : 0;

	std::vector<Chunk*> buffer;
	switch (m_shiftWorldQueue.front().Count)
	{
	case 0:
		{
			if (p_axis == 0)
			{
				m_xChunkOffset += p_shiftDirection;
			}
			else if (p_axis == 2)
			{
				m_zChunkOffset += p_shiftDirection;
			}

			buffer.reserve(WORLD_ELEMENTS_COUNT);

			for (size_t i = 0; i < m_chunks.size(); i++)
			{
				auto coords = From1Dto3D(i);
				if (coords[p_axis] == indiceChunksToDelete)
				{
					DestroyChunk(i);
				}
			
				if (coords[p_axis] == indiceChunksToAdd)
				{
					buffer.emplace_back(new Chunk({
						(coords[0] + m_xChunkOffset - WORLD_SIZE / 2) * CHUNK_SIZE,
						(coords[1] * CHUNK_SIZE) ,
						(coords[2] + m_zChunkOffset - WORLD_SIZE / 2) * CHUNK_SIZE }));

					buffer.back()->UpdateBlocksToRender();
				}
				else
				{
					coords[p_axis] += p_shiftDirection;
					const uint64_t index = From3Dto1D(coords[0], coords[1], coords[2]);
					buffer.push_back(std::move(m_chunks[index]));
				}
			}
			m_chunks = std::move(buffer);
			SetNeighbors();
			buffer.clear();
		}
		break;

	case 1:
		{
			if (p_axis == 0)
			{
				for (uint8_t zChunk = 0; zChunk < WORLD_SIZE; zChunk++)
				{
					GenerateChunk(indiceChunksToAdd, zChunk);
				}
			}
			else if (p_axis == 2)
			{
				for (uint8_t xChunk = 0; xChunk < WORLD_SIZE; xChunk++)
				{
					GenerateChunk(xChunk, indiceChunksToAdd);
				}
			}
		}
		break;

	default:
		{
			if (p_axis == 0)
			{
				const int yChunk = WORLD_SIZE + 1 - m_shiftWorldQueue.front().Count;

				for (uint8_t zChunk = 0; zChunk < WORLD_SIZE; zChunk++)
				{
					SetNeighbor(indiceChunksToAdd - p_shiftDirection, yChunk, zChunk);
					SetNeighbor(indiceChunksToAdd, yChunk, zChunk);

					m_chunks[From3Dto1D(indiceChunksToAdd - p_shiftDirection, yChunk, zChunk)]->UpdateBlocksToRender();
					m_chunks[From3Dto1D(indiceChunksToAdd, yChunk, zChunk)]->UpdateBlocksToRender();
				}
			}
			else if (p_axis == 2)
			{
				const int yChunk = WORLD_SIZE + 1 - m_shiftWorldQueue.front().Count;

				for (uint8_t xChunk = 0; xChunk < WORLD_SIZE; xChunk++)
				{
					SetNeighbor(xChunk, yChunk, indiceChunksToAdd - p_shiftDirection);
					SetNeighbor(xChunk, yChunk, indiceChunksToAdd);

					m_chunks[From3Dto1D(xChunk, yChunk, indiceChunksToAdd - p_shiftDirection)]->UpdateBlocksToRender();
					m_chunks[From3Dto1D(xChunk, yChunk, indiceChunksToAdd)]->UpdateBlocksToRender();
				}
			}
		}
		break;
	}

	++m_shiftWorldQueue.front().Count;

	if (m_shiftWorldQueue.front().Count == WORLD_SIZE + 2)
	{
		m_shiftWorldQueue.pop();
	}
}

void AmberCraft::Terrain::World::UpdateChunksToRender() const
{
	for (size_t i = 0; i < m_chunks.size(); i++)
	{
		m_chunks[i]->UpdateBlocksToRender();
	}
}

void AmberCraft::Terrain::World::Draw(AmberEngine::Managers::RenderingManager& p_renderingManager) const
{
	const glm::mat4 projectionMatrix = p_renderingManager.CalculateProjectionMatrix();
	const glm::mat4 viewMatrix       = p_renderingManager.CalculateViewMatrix();

	auto& chunkShader = p_renderingManager.GetResourcesManager().GetShader("Chunk");

	p_renderingManager.GetResourcesManager().GetTexture("atlasBlocks").Bind();

	chunkShader.Bind();
	chunkShader.SetUniformMat4("projection", projectionMatrix);
	chunkShader.SetUniformMat4("view", viewMatrix);
	chunkShader.SetUniform1i("atlas", 0);
	
	for (const auto m_chunk : m_chunks)
	{
		const glm::vec3 chunkPosition = m_chunk->GetPosition();

		chunkShader.SetUniformMat4("model", glm::translate(glm::mat4(1.0f), chunkPosition));
		m_chunk->Draw();
	}
}

AmberCraft::Terrain::Chunk* AmberCraft::Terrain::World::CreateChunk(const glm::vec3& p_position) const
{
	return new Chunk(p_position);
}

void AmberCraft::Terrain::World::DestroyChunk(uint16_t p_index)
{
	auto& chunk = m_chunks[p_index];
	delete chunk;
	chunk = nullptr;

	m_chunks[p_index] = nullptr;
}

AmberCraft::Terrain::BlockData AmberCraft::Terrain::World::GetBlock(int16_t p_x, int16_t p_y, int16_t p_z) const
{
	const float xChunk = static_cast<float>(p_x) / CHUNK_SIZE + WORLD_SIZE / 2 - m_xChunkOffset;
	const float yChunk = static_cast<float>(p_y) / CHUNK_SIZE;
	const float zChunk = static_cast<float>(p_z) / CHUNK_SIZE + WORLD_SIZE / 2 - m_zChunkOffset;

	if (xChunk < 0 || xChunk >= WORLD_SIZE ||
		yChunk < 0 || yChunk >= WORLD_SIZE ||
		zChunk < 0 || zChunk >= WORLD_SIZE)
	{
		return BlockData{ EBlockType::AIR };
	}

	const uint16_t chunkElement = From3Dto1D(xChunk, yChunk, zChunk);
	const uint16_t blockElement = Chunk::From3Dto1D(
		static_cast<uint8_t>(p_x + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE,
		static_cast<uint8_t>(p_y) % CHUNK_SIZE,
		static_cast<uint8_t>(p_z + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE);

	return m_chunks[chunkElement]->blocks[blockElement];
}

void AmberCraft::Terrain::World::SetBlock(int16_t p_x, int16_t p_y, int16_t p_z, BlockData p_blockData, bool p_updateChunk) const
{
	const float xChunk = static_cast<float>(p_x) / CHUNK_SIZE + WORLD_SIZE / 2 - m_xChunkOffset;
	const float yChunk = static_cast<float>(p_y) / CHUNK_SIZE;
	const float zChunk = static_cast<float>(p_z) / CHUNK_SIZE + WORLD_SIZE / 2 - m_zChunkOffset;

	if (xChunk < 0 || xChunk >= WORLD_SIZE ||
		yChunk < 0 || yChunk >= WORLD_SIZE ||
		zChunk < 0 || zChunk >= WORLD_SIZE)
	{
		return;
	}

	const uint16_t chunkElement = From3Dto1D(xChunk, yChunk, zChunk);
	const uint16_t blockElement = Chunk::From3Dto1D(
		static_cast<uint8_t>(p_x + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE,
		static_cast<uint8_t>(p_y) % CHUNK_SIZE,
		static_cast<uint8_t>(p_z + WORLD_SIZE / 2 * CHUNK_SIZE) % CHUNK_SIZE);

	m_chunks[chunkElement]->blocks[blockElement] = p_blockData;

	if (p_updateChunk)
	{
		m_chunks[chunkElement]->UpdateBlocksToRender();
		m_chunks[chunkElement]->UpdateNeighBors();
	}
}

constexpr uint16_t AmberCraft::Terrain::World::From3Dto1D(uint8_t p_x, uint8_t p_y, uint8_t p_z)
{
	return p_x + p_y * WORLD_SIZE + p_z * WORLD_SIZE * WORLD_SIZE;
}

constexpr std::array<uint8_t, 3> AmberCraft::Terrain::World::From1Dto3D(uint16_t p_index)
{
	const uint8_t z = p_index / (WORLD_SIZE * WORLD_SIZE);
	p_index -= z * (WORLD_SIZE * WORLD_SIZE);
	const uint8_t x = p_index % WORLD_SIZE;
	const uint8_t y = p_index / WORLD_SIZE;

	return std::array<uint8_t, 3>({x, y, z});
}

std::array<int64_t, 3> AmberCraft::Terrain::World::GetChunkCoordinates(const glm::vec3& p_worldPosition)
{
	const int64_t xChunk = static_cast<uint64_t>(p_worldPosition.x) / CHUNK_SIZE + WORLD_SIZE / 2;
	const int64_t yChunk = static_cast<uint64_t>(p_worldPosition.y) / CHUNK_SIZE;
	const int64_t zChunk = static_cast<uint64_t>(p_worldPosition.z) / CHUNK_SIZE + WORLD_SIZE / 2;

	return { static_cast<int64_t>(xChunk), static_cast<int64_t>(yChunk), static_cast<int64_t>(zChunk) };
}
