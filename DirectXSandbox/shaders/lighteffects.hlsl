namespace LightFX
{
    struct AmbientLight
    {
        float3 ambientLightposition;
        float ambientLightstrength;
        float3 ambientLightdirection;
        float _padding;
        float3 ambientLightcolor;
    };

    struct PointLight
    {
        float3 pointLightPosition;
        float pointLightStrength;
        float3 pointLightDirection;
        float _padding;
        float3 pointLightColor;
        float pointLightAttenuation;
        float pointLightMaximumCalcDistance;
    };

    struct SpotLight
    {
        float3 spotLightPosition;
        float spotLightStrength;
        float3 spotLightDirection;
        float _padding;
        float3 spotLightColor;
        float spotLightDistance;
        float attenuation;
    };
    
    bool computePointLight(float3 pixelWorldPos, float3 normal, PointLight inPointLight,
        out float3 outPointLight)
    {
        float distanceToLight = distance(inPointLight.pointLightPosition, pixelWorldPos);
        if (distanceToLight > inPointLight.pointLightMaximumCalcDistance)
            return false;
        
        float3 vecToLight = normalize(inPointLight.pointLightPosition - pixelWorldPos);
        float3 pointLightIntensity = max(dot(vecToLight, normal), 0);
        outPointLight = pointLightIntensity * (inPointLight.pointLightStrength / (distanceToLight * inPointLight.pointLightAttenuation)) * inPointLight.pointLightColor;
        
        return true;
    }
    
    bool computeSpotLight(float3 pixelWorldPos, float3 normal, float3 eyePos, SpotLight inSpotLight,
        out float3 outSpotLightAmbient, out float3 outSpotLightDiffuse, out float3 outSpotLightSpec)
    {
        outSpotLightAmbient = float3(0, 0, 0);
        outSpotLightDiffuse = float3(0, 0, 0);
        outSpotLightSpec = float3(0, 0, 0);

        float3 toEye = normalize(eyePos - pixelWorldPos);

        float3 lightVec = inSpotLight.spotLightPosition - pixelWorldPos;
        float d = length(lightVec);

        if (d > inSpotLight.spotLightDistance)
            return false;

        lightVec /= d;

        float diffFactor = dot(lightVec, normal);

        [flatten]
        if (diffFactor > 0.f)
        {
            float3 v = reflect(-lightVec, normal);
            float specFactor = pow(max(dot(v, toEye), 0.f), 1.f);

            outSpotLightDiffuse = diffFactor;
            outSpotLightSpec = specFactor * inSpotLight.spotLightDistance;
        }

        float spot = pow(max(dot(-lightVec, inSpotLight.spotLightDirection), 0.0f), inSpotLight.spotLightStrength);
        float att = spot / (d * inSpotLight.attenuation);

        outSpotLightDiffuse *= att;
        outSpotLightSpec *= att;
    
        return true;
    }
}
