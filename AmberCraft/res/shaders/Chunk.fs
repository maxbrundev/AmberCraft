#version 460 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 Normal;

in float Visibility;
in flat vec3 shadowFilter;

uniform sampler2D atlas;
uniform vec3 skyColor;

void main()
{
    vec3 norm = normalize(Normal);
    //FragColor = vec4(norm * 0.5 + 0.5, 1.0);

    FragColor = texture(atlas, TexCoord) * vec4(shadowFilter, 1.0);
    FragColor = mix(vec4(skyColor, 1.0), FragColor, Visibility);
}
