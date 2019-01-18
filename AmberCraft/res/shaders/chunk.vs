#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aNormal;
layout (location = 1) in vec2 aTexCoord;

layout(std430, binding = 0) buffer BlockData
{
    uint data[];
};

out vec3 FragPos;
out vec2 TexCoord;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D texture1;
uniform bool disableShadows;

out flat uint blockType;
out flat vec3 shadowFilter;

const ivec2 c_atlasSize = textureSize(texture1, 0);
const float c_textureResolution = 16.0f;
const float c_textureWidth = c_atlasSize.x / c_textureResolution;
const float c_textureHeight = c_atlasSize.y / c_textureResolution;

const float c_texelBleedingOffset = 0.00001f;

vec3 BlockOffset;

const vec3 chunkHalfSize = vec3(8.0f, 8.0f, 8.0f);

vec3 CalculateBlockOffset()
{
	float posX = float(data[gl_InstanceID] % 16);
    float posY = float((data[gl_InstanceID] >> 8) % 16);
    float posZ = float((data[gl_InstanceID] >> 16) % 16);

    return vec3(posX, posY, posZ);
}

bool IsTopFace()
{
    return aNormal.y > 0;
}

bool IsBottomFace()
{
    return aNormal.y < 0;
}

bool IsSideFace()
{
    return aNormal.y == 0;
}

bool IsXFace()
{
    return aNormal.y == 0 && aNormal.x != 0;
}

bool IsZFace()
{
    return aNormal.y == 0 && aNormal.z != 0;
}


vec2 GetTexture(uint p_x, uint p_y)
{
    vec2 result = aTexCoord;
    
    result.x /= c_textureWidth;
    result.y /= c_textureHeight;

    result.x += p_x / c_textureWidth;
    result.y += (c_textureHeight - 1 - p_y) / c_textureHeight;

	if (aTexCoord.x == 0)
        result.x += c_texelBleedingOffset;
    if (aTexCoord.x == 1)
        result.x -= c_texelBleedingOffset;
    if (aTexCoord.y == 0)
        result.y += c_texelBleedingOffset;
    if (aTexCoord.y == 1)
        result.y -= c_texelBleedingOffset;

    return result;
}

vec2 GetTripleTexture(uint p_topX, uint p_topY, uint p_bottomX, uint p_bottomY, uint p_sideX, uint p_sideY)
{
            if (IsTopFace())       return GetTexture(p_topX,    p_topY);
    else    if (IsBottomFace())    return GetTexture(p_bottomX, p_bottomY);
    else    if (IsSideFace())      return GetTexture(p_sideX,   p_sideY);
}

vec2 CalculateAtlasTextureCoordinates()
{
    switch (blockType)
    {
        case 1: return GetTripleTexture(0,0, 2,0, 1,0);     // 01: GRASS
        case 2: return GetTexture(2,0);                     // 02: DIRT
        case 3: return GetTexture(0,1);                     // 03: STONE
    }

    return vec2(0, 0);
}

vec3 CalculateShadows()
{
	float shadowCoefficient = 1.0f;

	if(IsXFace())
		shadowCoefficient = 0.9f;

	if(IsZFace())
		shadowCoefficient = 0.8f;

	if(IsBottomFace())
		shadowCoefficient = 0.6f;

	if(IsTopFace())
		shadowCoefficient = 1.0f;

	return vec3(shadowCoefficient, shadowCoefficient, shadowCoefficient);
}

void main()
{
	blockType = (data[gl_InstanceID] >> 24) % 16;
	BlockOffset = CalculateBlockOffset();
	FragPos = vec3(model * vec4(aPos + BlockOffset, 1.0));

	Normal = mat3(transpose(inverse(model))) * aNormal;  

    TexCoord = CalculateAtlasTextureCoordinates();

	if(!disableShadows)
		shadowFilter = CalculateShadows();
	else
		shadowFilter = vec3(1.0f, 1.0f, 1.0f);

	gl_Position = projection * view * vec4(FragPos, 1.0);
}