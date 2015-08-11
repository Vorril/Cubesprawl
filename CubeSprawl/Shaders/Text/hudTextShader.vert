#version 330
layout(location = 0)in vec4 vert;
layout(location = 1) in vec2 vertUV;

out vec2 UV;
void main()
{
    gl_Position = vert;
    UV = vertUV;
}