#version 330
in vec2 UV;
in float incidence;

out vec3 fragColor;

uniform sampler2D diffuseSampler;

void main()
{
    vec3 diffCol = texture2D(diffuseSampler, UV).rgb;
    fragColor = 0.7*(diffCol * incidence)+ 0.3*diffCol;
}