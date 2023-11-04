#shader vertex
#version 430 core

layout (location = 0) in vec3 geo_Pos;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

vec3 FragPos;

void main()
{
	FragPos = vec3(u_Model * vec4(geo_Pos, 1.0));
	gl_Position = u_Projection * u_View * vec4(FragPos, 1.0);
}

#shader fragment
#version 430 core

out vec4 FragColor;

void main()
{
	FragColor = vec4(1.f, 0.3f, 0.f, 1.f);
} 