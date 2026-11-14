#version 330 core
in vec2 UV;
out vec4 FragColor;
uniform sampler2D font;     // single-channel glyph atlas
uniform vec4 color;
void main()
{
    float a = texture(font, UV).r;
    if (a < 0.5) discard;
    FragColor = color;
}
