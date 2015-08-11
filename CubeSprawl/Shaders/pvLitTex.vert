#version 330
layout(location = 0)in vec4 vert;
layout(location = 1)in vec2 vertUV;
layout(location = 2)in vec3 norm;

out vec2 UV;
out float incidence;

layout(std140) uniform GlobalPV
{
mat4 p;
mat4 v;
mat4 pv;
vec4 l;
};

void main()
{
    gl_Position = pv * vert;
    UV = vertUV;
    incidence = dot(l.rgb, norm)*l.a;
    incidence = max(incidence, 0.0);
}