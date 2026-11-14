#version 330 core
layout (location = 0) in vec2 aPos;   // pixels, origin = top-left corner
layout (location = 1) in vec2 aUV;
out vec2 UV;
uniform mat4 ortho;
void main()
{
    UV = aUV;
    gl_Position = ortho * vec4(aPos, 0.0, 1.0);
}
