#version 460 core
out vec4 FragColor;

in vec2 TexCoord;

void main() 
{
	float alpha = 1.0;
    
	alpha = TexCoord.x > 0.01f && TexCoord.y > 0.01f && TexCoord.x < 0.99f && TexCoord.y < 0.99f ? 0.0 : 1.0;
    gl_FragDepth = gl_FragCoord.z;

    FragColor = vec4(1.0, 1.0, 1.0, alpha);
}