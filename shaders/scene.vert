#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aUV;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec2 uvScale;     // texture repeats per metre
uniform bool worldUV;      // false = plain 0..1 UVs (used for the hidden UV board)

void main()
{
    vec4 world = model * vec4(aPos, 1.0);
    FragPos   = world.xyz;
    Normal    = mat3(transpose(inverse(model))) * aNormal;
    // Tile the texture by the real size of the face, so it never looks stretched,
    // however much the cube was scaled. U runs along the first, V along the second dimension.
    vec3 s  = vec3(length(model[0].xyz), length(model[1].xyz), length(model[2].xyz));
    vec3 an = abs(aNormal);
    vec2 dims = (an.x > 0.5) ? vec2(s.z, s.y) : (an.y > 0.5) ? vec2(s.x, s.z) : vec2(s.x, s.y);
    TexCoords = worldUV ? aUV * dims * uvScale : aUV;
    gl_Position = projection * view * world;
}
