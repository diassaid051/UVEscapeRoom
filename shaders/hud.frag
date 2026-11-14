#version 330 core
in vec2 UV;
out vec4 FragColor;
uniform sampler2D font;     // single-channel glyph atlas
uniform vec4 color;
uniform bool solid;        // true = plain filled rectangle (no glyph lookup)
void main()
{
    float a = solid ? 1.0 : texture(font, UV).r;
    if (a < 0.5) discard;
    FragColor = color;
}
