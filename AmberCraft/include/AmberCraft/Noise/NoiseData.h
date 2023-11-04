#pragma once

namespace AmberCraft::Noise
{
	struct NoiseData
	{
		uint16_t Seed;
		uint8_t  MaxHeight;
		uint8_t  AdditionalElevation;
		uint8_t  Octaves;
		float    Scale;
		float    Persistence;
		float    Lacunarity;
		float    Redistribution;
		float    HeightScale;

		NoiseData(uint16_t p_seed = 540, uint8_t p_maxHeight = 80, uint8_t p_additionalElevation = 10, uint8_t p_octaves = 6,
		float p_scale = 35.0f, float p_persistence = 0.01f, float p_lacunarity = 1.4f, float p_redistribution = 1.05f, float p_heightScale = 0.6f) :
			Seed{ p_seed },
			MaxHeight{ p_maxHeight },
			AdditionalElevation{ p_additionalElevation },
			Octaves{ p_octaves },
			Scale{ p_scale },
			Persistence{ p_persistence },
			Lacunarity{ p_lacunarity },
			Redistribution{ p_redistribution },
			HeightScale{ p_heightScale }
		{ }
	};
}
