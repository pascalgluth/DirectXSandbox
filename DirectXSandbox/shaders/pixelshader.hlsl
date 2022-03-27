#include "LightEffects.hlsl"

cbuffer ambientLightBuffer : register(b0)
{
    LightFX::AmbientLight cAmbientLight;
}

cbuffer pointLightBuffer : register(b1)
{
    LightFX::PointLight cPointLight;
}

cbuffer spotLightBuffer : register(b2)
{
    LightFX::SpotLight cSpotLight;
}

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 textureCoord : TEXCOORD;
    float3 worldPosition : WORLD_POSITION;
    float3 normal : NORMAL;
    float3 eyePos : EYEPOS;
};

Texture2D objectTexture : TEXTURE : register(t0);
SamplerState objectSampler : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampledColor = objectTexture.Sample(objectSampler, input.textureCoord);

    // Ambient Light
	float3 allLight = cAmbientLight.ambientLightcolor * cAmbientLight.ambientLightstrength;

    float3 pointLight;
    if (LightFX::computePointLight(input.worldPosition, input.normal, cPointLight,
        pointLight))
    {
        allLight += pointLight;
    }
    

    float3 spotLightAmbient;
    float3 spotLightDiffuse;
    float3 spotLightSpec;
    LightFX::computeSpotLight(input.worldPosition, input.normal, input.eyePos, cSpotLight,
        spotLightAmbient, spotLightDiffuse, spotLightSpec);
    
    allLight += spotLightAmbient;
    allLight += spotLightDiffuse;
    allLight += spotLightSpec;
    

    float3 pixelColor = sampledColor * allLight;

    return float4(pixelColor, 1.f);
}