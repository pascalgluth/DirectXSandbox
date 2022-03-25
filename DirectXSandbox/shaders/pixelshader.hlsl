#include "LightEffects.hlsl"

cbuffer ambientLightBuffer : register(b0)
{
    float3 ambientLightColor;
    float ambientLightStrength;
}

cbuffer pointLightBuffer : register(b1)
{
    float3 pointLightColor;
    float pointLightStrength;
    float3 pointLightPosition;
    float pointLightAttenuation;
    float pointLightMaximumCalcDistance;
}

cbuffer spotLightBuffer : register(b2)
{
    float3 spotLightPosition;
    float spotLightStrength;
    float3 spotLightDirection;
    float spotLightCutoff;
}

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 textureCoord : TEXCOORD;
    float3 worldPosition : WORLD_POSITION;
    float3 normal : NORMAL;
};

Texture2D objectTexture : TEXTURE : register(t0);
SamplerState objectSampler : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 sampledColor = objectTexture.Sample(objectSampler, input.textureCoord);

    // Ambient Light
	float3 allLight = ambientLightColor * ambientLightStrength;

    float3 pointLight;
    if (LightFX::computePointLight(input.worldPosition, input.normal,
        pointLightPosition, pointLightMaximumCalcDistance, pointLightStrength, pointLightColor, pointLightAttenuation,
        pointLight))
    {
        allLight += pointLight;
    }
    

    float3 spotLight;
    if (LightFX::computeSpotLight(input.worldPosition, input.normal,
        spotLightPosition, spotLightStrength, spotLightCutoff, spotLightDirection,
        spotLight))
    {
        allLight += spotLight;
    }

    float3 pixelColor = sampledColor * allLight;

    return float4(pixelColor, 1.f);
}