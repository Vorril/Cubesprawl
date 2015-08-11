#version 330
in vec2 UV;

out vec4 fragColor;

uniform sampler2D textHudSampler;

void main()
{
    vec3 diffCol = texture(textHudSampler, UV).rgb;
    fragColor = diffCol.rgbr;
}