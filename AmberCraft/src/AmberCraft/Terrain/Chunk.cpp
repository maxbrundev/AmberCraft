#include "AmberCraftPCH.h"

#include "AmberCraft/Terrain/Chunk.h"

#include "AmberCraft/Terrain/World.h"

bool AmberCraft::Terrain::Chunk::__CHUNK_SURFACE_CULLING = true;
bool AmberCraft::Terrain::Chunk::__BLOCK_SURFACE_CULLING = true;

AmberCraft::Terrain::Chunk::Chunk(const glm::vec3& p_position) : m_chunkCoordinatePosition(p_position)
{
	Fill(EBlockType::AIR);

	m_blocksToRender.reserve(CHUNK_ELEMENTS_COUNT);
}

void AmberCraft::Terrain::Chunk::Fill(EBlockType p_blockType)
{
	for (int i = 0; i < CHUNK_ELEMENTS_COUNT; i++)
	{
		blocks[i].type = p_blockType;
	}
}

void AmberCraft::Terrain::Chunk::SetChunksNeighbors(Chunk* p_left, Chunk* p_right, Chunk* p_top, Chunk* p_bot, Chunk* p_front, Chunk* p_back)
{
	m_chunksNeighbors.left  = p_left;
	m_chunksNeighbors.right = p_right;
	m_chunksNeighbors.top   = p_top;
	m_chunksNeighbors.bot   = p_bot;
	m_chunksNeighbors.front = p_front;
	m_chunksNeighbors.back  = p_back;

	m_isOccluded = m_chunksNeighbors.left != nullptr
	&& m_chunksNeighbors.right            != nullptr
	&& m_chunksNeighbors.top              != nullptr
	&& m_chunksNeighbors.bot              != nullptr
	&& m_chunksNeighbors.front            != nullptr
	&& m_chunksNeighbors.back             != nullptr;
		
}

void AmberCraft::Terrain::Chunk::UpdateNeighBors() const
{
	if (m_chunksNeighbors.left)
		m_chunksNeighbors.left->UpdateBlocksToRender();

	if (m_chunksNeighbors.right)
		m_chunksNeighbors.right->UpdateBlocksToRender();

	if (m_chunksNeighbors.top)
		m_chunksNeighbors.top->UpdateBlocksToRender();

	if (m_chunksNeighbors.bot)
		m_chunksNeighbors.bot->UpdateBlocksToRender();

	if (m_chunksNeighbors.back)
		m_chunksNeighbors.back->UpdateBlocksToRender();

	if (m_chunksNeighbors.front)
		m_chunksNeighbors.front->UpdateBlocksToRender();
}

bool AmberCraft::Terrain::Chunk::IsBlockOccluded(uint8_t p_x, uint8_t p_y, uint8_t p_z)
{
	BlockData* left  = GetBlock(p_x, p_y, p_z, ChunkSides::LEFT);
	BlockData* right = GetBlock(p_x, p_y, p_z, ChunkSides::RIGHT);
	BlockData* top   = GetBlock(p_x, p_y, p_z, ChunkSides::TOP);
	BlockData* bot   = GetBlock(p_x, p_y, p_z, ChunkSides::BOT);
	BlockData* front = GetBlock(p_x, p_y, p_z, ChunkSides::FRONT);
	BlockData* back  = GetBlock(p_x, p_y, p_z, ChunkSides::BACK);

	if(__CHUNK_SURFACE_CULLING)
	{
		//Draw only surface
		return (!left  || left->type  != EBlockType::AIR)
			&& (!right || right->type != EBlockType::AIR)
			&& (!top   || top->type   != EBlockType::AIR)
			&& (!bot   || bot->type   != EBlockType::AIR)
			&& (!front || front->type != EBlockType::AIR)
			&& (!back  || back->type  != EBlockType::AIR);
	}
	else
	{
		//Draw chunk walls
		return (left  && left->type  != EBlockType::AIR)
			&& (right && right->type != EBlockType::AIR)
			&& (top   && top->type   != EBlockType::AIR)
			&& (bot   && bot->type   != EBlockType::AIR)
			&& (front && front->type != EBlockType::AIR)
			&& (back  && back->type  != EBlockType::AIR);
	}
}

bool AmberCraft::Terrain::Chunk::IsOccluded() const
{
	return m_isOccluded;
}

glm::vec3 AmberCraft::Terrain::Chunk::GetPosition() const
{
	return m_chunkCoordinatePosition;
}

void AmberCraft::Terrain::Chunk::FillBlocksToRender()
{
	std::vector<uint32_t> buffer;
	buffer.reserve(CHUNK_ELEMENTS_COUNT);

	std::array<uint8_t, 3> blockCoordinates;
	uint16_t blockIndex;

	for (uint16_t i = 0; i < CHUNK_ELEMENTS_COUNT; i++)
	{
		blockCoordinates = From1Dto3D(i);
		blockIndex       = From3Dto1D(blockCoordinates[0], blockCoordinates[1], blockCoordinates[2]);

		uint32_t blockData = 0;
		blockData |= static_cast<uint32_t>(i % CHUNK_SIZE) & 0xFF;
		blockData |= (static_cast<uint32_t>(i / CHUNK_SIZE % CHUNK_SIZE) & 0xFF) << 8;
		blockData |= (static_cast<uint32_t>(i / (CHUNK_SIZE * CHUNK_SIZE)) & 0xFF) << 16;
		blockData |= (static_cast<uint32_t>(blocks[blockIndex].type) & 0xFF) << 24;

		if (!IsBlockOccluded(blockCoordinates[0], blockCoordinates[1], blockCoordinates[2]) && blockData >> 24 != 0)
		{
			buffer.push_back(blockData);
		}
	}

	m_blocksToRender = std::move(buffer);
}

void AmberCraft::Terrain::Chunk::UpdateBlocksToRender()
{
	FillBlocksToRender();

	m_chunkBuffers.SendBlocksToGPU(m_blocksToRender);
	m_blocksToRenderCount = static_cast<uint16_t>(m_blocksToRender.size());
}

void AmberCraft::Terrain::Chunk::Draw() const
{
	m_chunkBuffers.DrawChunk(m_blocksToRenderCount);
}

AmberCraft::Terrain::BlockData* AmberCraft::Terrain::Chunk::GetBlock(uint8_t p_x, uint8_t p_y, uint8_t p_z, ChunkSides p_chunkSide)
{
	const uint8_t min = 0;
	const uint8_t max = CHUNK_SIZE - 1;

	if(__BLOCK_SURFACE_CULLING)
	{
		//Return Block or Neighbor Block
		switch (p_chunkSide)
		{
		case ChunkSides::LEFT:
			return (p_x - 1 >= min)
			? &blocks[From3Dto1D(p_x - 1, p_y, p_z)]
			: (m_chunksNeighbors.left ? &m_chunksNeighbors.left->blocks[From3Dto1D(max, p_y, p_z)] : nullptr);
	
		case ChunkSides::RIGHT:
			return (p_x + 1 <= max)
			? &blocks[From3Dto1D(p_x + 1, p_y, p_z)]
			: (m_chunksNeighbors.right ? &m_chunksNeighbors.right->blocks[From3Dto1D(min, p_y, p_z)] : nullptr);
	
		case ChunkSides::BOT:
			return (p_y - 1 >= min)
			? &blocks[From3Dto1D(p_x, p_y - 1, p_z)]
			: (m_chunksNeighbors.bot ? &m_chunksNeighbors.bot->blocks[From3Dto1D(p_x, max, p_z)] : nullptr);
	
		case ChunkSides::TOP:
			return (p_y + 1 <= max)
			? &blocks[From3Dto1D(p_x, p_y + 1, p_z)]
			: (m_chunksNeighbors.top ? &m_chunksNeighbors.top->blocks[From3Dto1D(p_x, min, p_z)] : nullptr);
	
		case ChunkSides::BACK:
			return (p_z - 1 >= min)
			? &blocks[From3Dto1D(p_x, p_y, p_z - 1)]
			: (m_chunksNeighbors.back ? &m_chunksNeighbors.back->blocks[From3Dto1D(p_x, p_y, max)] : nullptr);
	
		case ChunkSides::FRONT:
			return (p_z + 1 <= max)
			? &blocks[From3Dto1D(p_x, p_y, p_z + 1)]
			: (m_chunksNeighbors.front ? &m_chunksNeighbors.front->blocks[From3Dto1D(p_x, p_y, min)] : nullptr);
	
		default:
			return nullptr;
		}
	}
	else
	{
		//Return Block
		switch (p_chunkSide)
		{
		case ChunkSides::LEFT:
			return (p_x - 1 >= min)
			? &blocks[From3Dto1D(p_x - 1, p_y, p_z)]
			: nullptr;
	
		case ChunkSides::RIGHT:
			return (p_x + 1 <= max)
			? &blocks[From3Dto1D(p_x + 1, p_y, p_z)]
			: nullptr;
	
		case ChunkSides::BOT:
			return (p_y - 1 >= min)
			? &blocks[From3Dto1D(p_x, p_y - 1, p_z)]
			: nullptr;
	
		case ChunkSides::TOP:
			return (p_y + 1 <= max)
			? &blocks[From3Dto1D(p_x, p_y + 1, p_z)]
			: nullptr;
	
		case ChunkSides::BACK:
			return (p_z - 1 >= min)
			? &blocks[From3Dto1D(p_x, p_y, p_z - 1)]
			: nullptr;
	
		case ChunkSides::FRONT:
			return (p_z + 1 <= max)
			? &blocks[From3Dto1D(p_x, p_y, p_z + 1)]
			:nullptr;
	
		default:
			return nullptr;
		}
	}
}

constexpr uint16_t AmberCraft::Terrain::Chunk::From3Dto1D(uint8_t p_x, uint8_t p_y, uint8_t p_z)
{
	return p_x + p_y * CHUNK_SIZE + p_z * CHUNK_SIZE * CHUNK_SIZE;
}

constexpr std::array<uint8_t, 3> AmberCraft::Terrain::Chunk::From1Dto3D(uint16_t p_index)
{
	const uint8_t z = p_index / (CHUNK_SIZE * CHUNK_SIZE);
	p_index -= (z * (CHUNK_SIZE * CHUNK_SIZE));

	const uint8_t y = p_index / CHUNK_SIZE;
	const uint8_t x = p_index % CHUNK_SIZE;

	return std::array<uint8_t, 3>({x,y,z});
}
