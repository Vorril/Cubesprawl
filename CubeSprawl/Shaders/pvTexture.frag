#version 330
in vec2 UV;

out vec3 fragColor;

uniform sampler2D diffuseSampler;

void main()
{
    fragColor = texture2D(diffuseSampler, UV).rgb;
}