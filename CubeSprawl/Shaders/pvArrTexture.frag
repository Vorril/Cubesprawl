#version 330
in vec2 UV;
flat in float layer;

out vec3 fragColor;

uniform sampler2DArray arrSampler;

void main()
{
    fragColor = texture2DArray(arrSampler, vec3(UV, layer)).rgb;
}