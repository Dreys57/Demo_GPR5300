#version 450 core

layout(location = 0) out vec4 FragColor;

in vec3 out_position;
in vec3 out_normal;
in vec2 out_tex;
in vec3 out_camera;

uniform sampler2D Diffuse;
uniform sampler2D Specular;

const float ambient_strengh = 0.1;
const float specular_strength = 2;
const float specular_pow = 32;
const vec3 light_color = vec3(1.0, 1.0, 1.0);
const vec3 light_position = vec3(0.0, 0.0, 3.0);

void main()
{
    // Compute ambiant light.
    vec3 ambient = ambient_strengh * light_color;

    // Compute diffuse light.
    vec3 normal = normalize(out_normal);
    vec3 light_direction = normalize(light_position - out_position);
    float diff = max(dot(out_normal, light_direction), 0.0);
    vec3 diffuse = diff * light_color;

    // Compute specular light.
    vec3 view_direction = normalize(out_camera - out_position);
    vec3 reflection_direction = reflect(-light_direction, out_normal);
    float spec = pow(max(dot(view_direction, reflection_direction), 0.0), specular_pow);
    vec3 specular = specular_strength * spec * light_color;

    // Total light.
    vec3 result_diffuse_ambient = 
        (ambient + diffuse) * texture(Diffuse, out_tex).rgb; 
    vec3 result_specular =
        specular * texture(Specular, out_tex).r;
    FragColor = vec4(result_diffuse_ambient + result_specular, 1.0);
    // FragColor = vec4(normal, 1.0);
}