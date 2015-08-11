#version 330
layout(location = 0)in vec4 vert;
layout(location = 1)in vec2 vertUV;
layout(location = 2)in float vertLay;

out vec2 UV;
flat out float layer;

layout(std140) uniform GlobalPV
{
mat4 p;
mat4 v;
mat4 pv;
vec4 l;
};

void main()
{
    gl_Position = pv * vec4(vert.xyz, 1.0);
    UV = vertUV;
    layer = vertLay;
}