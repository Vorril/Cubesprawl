#version 330
in vec2 UV;
flat in float layer;
in vec4 lightDir_tangentSpace;

out vec3 fragColor;

uniform float ratio;
uniform sampler2DArray arrSampler;
uniform sampler2DArray normalSampler;

void main()
{
    vec3 textureNormal_tangentSpace = normalize(texture2DArray(normalSampler, vec3(UV, layer)).rgb*2.0 - 1.0);
    float incidence = clamp( dot(lightDir_tangentSpace.rgb, textureNormal_tangentSpace), 0.0, 1.0);
    
    vec3 diffColor = texture2DArray(arrSampler, vec3(UV, layer)).rgb;
    fragColor = diffColor*( ratio*(incidence * lightDir_tangentSpace.a) + (1.0 - ratio));
}