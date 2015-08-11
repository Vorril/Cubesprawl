#version 330
layout(location = 0)in vec4 vert;
layout(location = 1)in vec3 center;

layout(std140) uniform GlobalPV
{
mat4 p;
mat4 v;
mat4 pv;
vec4 l;
};

void main()
{
    gl_Position = pv * (vec4(center,0.0)+vert);
}