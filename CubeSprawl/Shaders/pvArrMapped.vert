#version 430
layout(location = 0)in vec4 vert;
layout(location = 1)in vec2 vertUV;
layout(location = 2)in float vertLay;
layout(location = 3)in vec3 tangent;
layout(location = 4)in vec3 bittangent;
layout(location = 5)in vec3 normal;

out vec2 UV;
flat out float layer;
out vec4 lightDir_tangentSpace;

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

    mat3 TBN = transpose(mat3(tangent, bittangent, normal));
    lightDir_tangentSpace = vec4(normalize( TBN * l.rgb ), l.a);
}