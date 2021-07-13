#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec2 aTex;

out vec3 out_color;
out vec2 out_tex;

void main()
{
    gl_Position = vec4(aPos, 1.0);
    out_color = aColor;
    out_tex = aTex;
}