#version 330
in vec2 UV;
in float incidence;

out vec3 fragColor;

uniform sampler2D diffuseSampler;
uniform float lambert;
uniform float diffuse;

void main()
{
    vec3 diffCol = texture2D(diffuseSampler, UV).rgb;
    fragColor = lambert*(diffCol * incidence)+ diffuse*diffCol;
}