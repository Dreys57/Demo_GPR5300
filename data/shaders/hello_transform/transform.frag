#version 450 core

layout(location = 0) out vec4 FragColor;

in vec3 out_color;
in vec2 out_tex;

uniform sampler2D textureDiffuse;

void main()
{
    FragColor = vec4(out_color, 1.0) * texture(textureDiffuse, out_tex);
}