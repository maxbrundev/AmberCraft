#include "pch.h"

#include "AmberCraft/Chunk.h"

#include "AmberCraft/World.h"

AmberCraft::Chunk::Chunk(const glm::ivec3& p_position) : m_chunkCoordinatePosition(p_position)
{
	Fill(BlockType::AIR);
}

void AmberCraft::Chunk::Fill(BlockType p_blockType)
{
	for (int i = 0; i < CHUNK_ELEMENTS_COUNT; i++)
	{
		blocks[i].type = p_blockType;
	}
}

void AmberCraft::Chunk::SetChunksNeighbors(Chunk* p_left, Chunk* p_right, Chunk* p_top, Chunk* p_bot, Chunk* p_front, Chunk* p_back)
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

void AmberCraft::Chunk::UpdateNeighBors() const
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

bool AmberCraft::Chunk::IsBlockOccluded(uint8_t p_x, uint8_t p_y, uint8_t p_z)
{
	BlockData* left  = GetBlock(p_x, p_y, p_z, ChunkSides::LEFT);
	BlockData* right = GetBlock(p_x, p_y, p_z, ChunkSides::RIGHT);
	BlockData* top   = GetBlock(p_x, p_y, p_z, ChunkSides::TOP);
	BlockData* bot   = GetBlock(p_x, p_y, p_z, ChunkSides::BOT);
	BlockData* front = GetBlock(p_x, p_y, p_z, ChunkSides::FRONT);
	BlockData* back  = GetBlock(p_x, p_y, p_z, ChunkSides::BACK);

	//Draw only surface
	return (!left || left->type != BlockType::AIR)
	&& (!right || right->type != BlockType::AIR)
	&& (!top || top->type != BlockType::AIR)
	&& (!bot || bot->type != BlockType::AIR)
	&& (!front || front->type != BlockType::AIR)
	&& (!back || back->type != BlockType::AIR);

	//Draw chunk walls
	//return (left && left->type != BlockType::AIR)
	//&& (right && right->type != BlockType::AIR)
	//&& (top && top->type != BlockType::AIR)
	//&& (bot && bot->type != BlockType::AIR)
	//&& (front && front->type != BlockType::AIR)
	//&& (back && back->type != BlockType::AIR);
}

bool AmberCraft::Chunk::IsOccluded() const
{
	return m_isOccluded;
}

std::vector<uint32_t> AmberCraft::Chunk::FillBlocksToRender()
{
	std::vector<uint32_t> blocksToRender;

	for (int i = 0; i < CHUNK_ELEMENTS_COUNT; i++)
	{
		auto blockCoordinates = From1Dto3D(i);

		union
		{
			uint32_t data;
			uint8_t bytes[4];

		}blockData;

		blockData.bytes[0] = static_cast<uint8_t>(i % CHUNK_SIZE);
		blockData.bytes[1] = static_cast<uint8_t>((i / CHUNK_SIZE) % CHUNK_SIZE);
		blockData.bytes[2] = static_cast<uint8_t>(i / (CHUNK_SIZE * CHUNK_SIZE));
		blockData.bytes[3] = static_cast<uint8_t>(blocks[From3Dto1D(blockCoordinates[0], blockCoordinates[1], blockCoordinates[2])].type);

		if (!IsBlockOccluded(blockCoordinates[0], blockCoordinates[1], blockCoordinates[2]) && blockData.bytes[3] != 0)
		{
			blocksToRender.push_back(blockData.data);
		}
	}

	return blocksToRender;
}

void AmberCraft::Chunk::UpdateBlocksToRender()
{
	const std::vector<uint32_t> blocksToRender = FillBlocksToRender();

	m_chunkBuffers.SendBlocksToGPU(blocksToRender);
	m_blocksToRenderCount = static_cast<uint16_t>(blocksToRender.size());
}

void AmberCraft::Chunk::Draw() const
{
	m_chunkBuffers.DrawChunk(m_blocksToRenderCount);
}

AmberCraft::BlockData* AmberCraft::Chunk::GetBlock(uint8_t p_x, uint8_t p_y, uint8_t p_z, ChunkSides p_chunkSide)
{
	const uint8_t min = 0;
	const uint8_t max = CHUNK_SIZE - 1;

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

	//Return Block
	//switch (p_chunkSide)
	//{
	//case ChunkSides::LEFT:
	//	return (p_x - 1 >= min)
	//	? &blocks[From3Dto1D(p_x - 1, p_y, p_z)]
	//	: nullptr;
	//
	//case ChunkSides::RIGHT:
	//	return (p_x + 1 <= max)
	//	? &blocks[From3Dto1D(p_x + 1, p_y, p_z)]
	//	: nullptr;
	//
	//case ChunkSides::BOT:
	//	return (p_y - 1 >= min)
	//	? &blocks[From3Dto1D(p_x, p_y - 1, p_z)]
	//	: nullptr;
	//
	//case ChunkSides::TOP:
	//	return (p_y + 1 <= max)
	//	? &blocks[From3Dto1D(p_x, p_y + 1, p_z)]
	//	: nullptr;
	//
	//case ChunkSides::BACK:
	//	return (p_z - 1 >= min)
	//	? &blocks[From3Dto1D(p_x, p_y, p_z - 1)]
	//	: nullptr;
	//
	//case ChunkSides::FRONT:
	//	return (p_z + 1 <= max)
	//	? &blocks[From3Dto1D(p_x, p_y, p_z + 1)]
	//	: nullptr;
	//
	//default: 
	//	return nullptr;
	//}
}

glm::ivec3 AmberCraft::Chunk::GetChunkCoordinatePosition() const
{
	return m_chunkCoordinatePosition;
}

glm::ivec3 AmberCraft::Chunk::GetWorldCoordinatePosition() const
{
	glm::ivec3 chunkPosition = GetChunkCoordinatePosition();

	chunkPosition.x = (chunkPosition.x - WORLD_SIZE / 2) * CHUNK_SIZE;
	chunkPosition.y = chunkPosition.y * CHUNK_SIZE;
	chunkPosition.z = (chunkPosition.z - WORLD_SIZE / 2) * CHUNK_SIZE;

	return chunkPosition;
}

void AmberCraft::Chunk::SetPosition(const glm::ivec3& p_position)
{
	m_chunkCoordinatePosition = p_position;
}

std::array<uint8_t, 3> AmberCraft::Chunk::From1Dto3D(uint16_t p_index)
{
	std::array<uint8_t, 3> result;

	result[2] = p_index / (CHUNK_SIZE * CHUNK_SIZE);
	p_index -= (std::get<2>(result) * (CHUNK_SIZE * CHUNK_SIZE));

	result[1] = p_index / CHUNK_SIZE;
	result[0] = p_index % CHUNK_SIZE;

	return result;
}

uint16_t AmberCraft::Chunk::From3Dto1D(uint8_t p_x, uint8_t p_y, uint8_t p_z)
{
	return p_x + p_y * CHUNK_SIZE + p_z * CHUNK_SIZE * CHUNK_SIZE;
}
