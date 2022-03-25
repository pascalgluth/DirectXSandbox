namespace LightFX
{
    bool computePointLight(float3 pixelWorldPos, float3 normal,
        float3 pointLightPosition, float pointLightRange, float pointLightStrength, float3 pointLightColor, float pointLightAttenuation,
        out float3 pointLight)
    {
        float distanceToLight = distance(pointLightPosition, pixelWorldPos);
        if (distanceToLight > pointLightRange)
            return false;
        
        float3 vecToLight = normalize(pointLightPosition - pixelWorldPos);
        float3 pointLightIntensity = max(dot(vecToLight, normal), 0);
        pointLight = pointLightIntensity * (pointLightStrength / (distanceToLight * pointLightAttenuation)) * pointLightColor;
        
        return true;
    }
    
    bool computeSpotLight(float3 pixelWorldPos, float3 normal,
        float3 spotLightPosition, float spotLightStrength, float spotLightCutoff, float3 spotLightDirection,
        out float3 spotLight)
    {
        // todo: Spot Light
        
        spotLight = float3(0, 0, 0);
    
        return true;
    }
}
