#version 330 core

in vec4 inColor;
in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D texSampler;
uniform sampler2D texSampler1;

void main()
{
    FragColor = mix(texture(texSampler, texCoord), texture(texSampler1, texCoord), 0.2) * inColor;
}
