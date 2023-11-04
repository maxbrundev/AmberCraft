#pragma once

#include "AmberCraft/Terrain/BlockData.h"

namespace AmberCraft::Terrain
{
	struct TerrainLayer
	{
		std::string Name;
		BlockData BlockData;
		uint8_t Height;

		TerrainLayer(std::string p_name, EBlockType p_blockID, uint8_t p_height) :
			Name{ std::move(p_name) },
			BlockData{ p_blockID },
			Height{ p_height }
		{ }
	};
}