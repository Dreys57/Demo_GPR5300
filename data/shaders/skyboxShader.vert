#version 450 core

layout(location = 0) in vec3 aPos;

out vec3 TexCoords;
out vec3 Normal;
out vec3 Position;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    vec4 pos = projection * view * vec4(aPos, 1.0f);
    gl_Position = pos.xyww;
}