#version 330
in vec2 UV;

out vec4 fragColor;

uniform sampler2D textSampler;

void main()
{
    vec3 diffCol = texture(textSampler, UV).rgb;
    fragColor = diffCol.rgbr;
}