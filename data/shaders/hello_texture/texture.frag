#version 450 core

layout(location = 0) out vec4 FragColor;

in vec3 out_color;
in vec2 out_tex;

uniform sampler2D textureDiffuse;
uniform sampler2D textureSmily;

void main()
{
    vec2 tex_coord_rect = vec2(out_tex.x, 1.0 - out_tex.y);
    FragColor = 
        texture(textureSmily, tex_coord_rect) + vec4(out_color, 1.0) * 
        texture(textureDiffuse, out_tex);
}