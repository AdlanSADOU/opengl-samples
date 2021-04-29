#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec4 inColor;
out vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float i;

void main()
{

   gl_Position = projection * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0);
   inColor = aColor;
   texCoord = aTexCoord;
}
