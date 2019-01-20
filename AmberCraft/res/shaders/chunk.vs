#version 460 core
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

out float Visibility;
out flat vec3 shadowFilter;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform sampler2D atlas;
uniform bool disableShadows;

const ivec2 atlasSize = textureSize(atlas, 0);
const float textureResolution = 16.0f;
const float textureWidth = atlasSize.x / textureResolution;
const float textureHeight = atlasSize.y / textureResolution;
const float texelBleedingOffset = 0.00001f;

const float density = 0.008f;
const float gradient = 1.5f;

vec3 blockOffset;
uint blockType;

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

  result.x /= textureWidth;
  result.y /= textureHeight;

  result.x += p_x / textureWidth;
  result.y += (textureHeight - 1 - p_y) / textureHeight;

  if (aTexCoord.x == 0)
        result.x += texelBleedingOffset;
    if (aTexCoord.x == 1)
        result.x -= texelBleedingOffset;
    if (aTexCoord.y == 0)
        result.y += texelBleedingOffset;
    if (aTexCoord.y == 1)
        result.y -= texelBleedingOffset;

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
  case 4: return GetTexture(2,3);                    // 04: BRICK
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
	vec4 positionRelativeTocam = view * model * vec4(aPos, 1.0);

	float distance = length(positionRelativeTocam.xyz);
	Visibility = exp(-pow((distance * density), gradient));
	Visibility = clamp(Visibility, 0.0, 1.0);

	blockType = (data[gl_InstanceID] >> 24) % 16;
	blockOffset = CalculateBlockOffset();
	FragPos = vec3(model * vec4(aPos + blockOffset, 1.0));

	Normal = mat3(transpose(inverse(model))) * aNormal;

	TexCoord = CalculateAtlasTextureCoordinates();

	if(!disableShadows)
		shadowFilter = CalculateShadows();
	else
		shadowFilter = vec3(1.0f, 1.0f, 1.0f);

	gl_Position = projection * view * vec4(FragPos, 1.0);
}
