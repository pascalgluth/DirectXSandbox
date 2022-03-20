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

    // Point Light
    float distanceToLight = distance(pointLightPosition, input.worldPosition);
    if (distanceToLight <= pointLightMaximumCalcDistance)
    {
        float3 vecToLight = normalize(pointLightPosition - input.worldPosition);
        float3 pointLightIntensity = max(dot(vecToLight, input.normal), 0);
        float3 pointLight = pointLightIntensity * (pointLightStrength / (distanceToLight * pointLightAttenuation)) * pointLightColor;
        allLight += pointLight;
    }

    float3 pixelColor = sampledColor * allLight;

    return float4(pixelColor, 1.f);
}