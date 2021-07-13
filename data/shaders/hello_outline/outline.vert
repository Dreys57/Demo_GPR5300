#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // mat3 inv_model = mat3(transpose(inverse(model)));
    mat4 pvm = projection * view * model;
    gl_Position = pvm * vec4(aPos, 1.0);
}