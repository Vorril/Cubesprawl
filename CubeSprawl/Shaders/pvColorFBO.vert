#version 330
layout(location = 1)in vec4 vert;
flat out float colID;

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
    colID = vert.w;
}