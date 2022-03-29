#include "LightEffects.hlsl"

cbuffer ambientLightBuffer : register(b0)
{
    LightFX::AmbientLight ambientLight;
}

cbuffer pointLightBuffer : register(b1)
{
    LightFX::PointLight pointLight;
}

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 textureCoord : TEXCOORD;
    float3 worldPosition : WORLD_POSITION;
    float3 normal : NORMAL;
    float3 eyePos : EYEPOS;
};

Texture2D diffTex : DIFF_TEXTURE : register(t0);
Texture2D specTex : SPEC_TEXTURE : register(t1);

SamplerState objectSampler : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 diffuseColor = diffTex.Sample(objectSampler, input.textureCoord);
    float4 specColor = specTex.Sample(objectSampler, input.textureCoord);

    // Ambient Light
	//float3 allLight = cAmbientLight.ambientLightcolor * cAmbientLight.ambientLightstrength;

    float4 diffuseLight = ambientLight.ambient;
    
    float4 a, d, s;

    LightFX::computePointLight(input.worldPosition, input.normal, input.eyePos, pointLight,
         diffuseColor, specColor, a, d, s);

    diffuseLight += d;
    float4 specularLight = s;

    /*float3 spotLightAmbient;
    float3 spotLightDiffuse;
    float3 spotLightSpec;
    LightFX::computeSpotLight(input.worldPosition, input.normal, input.eyePos, cSpotLight,
        spotLightAmbient, spotLightDiffuse, spotLightSpec);
    
    allLight += spotLightAmbient;
    allLight += spotLightDiffuse;
    allLight += spotLightSpec; */
    float4 color = (diffuseColor * diffuseLight) + (specColor * specularLight);

    return color;
}