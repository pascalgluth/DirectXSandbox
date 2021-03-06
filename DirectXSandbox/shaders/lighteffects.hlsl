namespace LightFX
{
    struct AmbientLight
    {
        float4 ambient;     // 16 byte
        float4 diffuse;     // 16 byte
        float4 specular;    // 16 byte
        
        // 48 byte
    };

    struct PointLight
    {
        float4 ambient;     // 16 byte
        float4 diffuse;     // 16 byte
        float4 specular;    // 16 byte        

        float3 position;    // 12 byte
        float range;        // 4 byte

        float3 attenuation; // 12 byte
        float padding;      // 4 byte

        // 80 byte
    };

    struct SpotLight
    {
        // todo: spot light
        float3 spotLightPosition;
        float spotLightStrength;
        float3 spotLightDirection;
        float _padding;
        float3 spotLightColor;
        float spotLightDistance;
        float attenuation;
    };

    void computePointLight(float3 pixelWorldPos, float3 normal, float3 eyePos, PointLight inPointLight,
                           out float4 outAmbient, out float4 outDiffuse, out float4 outSpecular)
    {
        outAmbient = float4(0.0f, 0.0f, 0.0f, 0.0f);
        outDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
        outSpecular = float4(0.0f, 0.0f, 0.0f, 0.0f);

        float distanceToLight = distance(inPointLight.position, pixelWorldPos);

        if (distanceToLight > inPointLight.range)
            return;

        float3 vectorToLight = normalize(inPointLight.position - pixelWorldPos);
        float diffuseFactor = max(dot(vectorToLight, normal), 0);
        //float diffuseAttenuation = 1.f / (inPointLight.attenuation.x + inPointLight.attenuation.y * distanceToLight + inPointLight.attenuation.z * pow(distanceToLight, 2));
        diffuseFactor = diffuseFactor / (distanceToLight * inPointLight.attenuation.x);
        outDiffuse = inPointLight.diffuse * diffuseFactor;

        float3 viewDirection = normalize(eyePos - pixelWorldPos);
        float3 reflectDirection = reflect(-vectorToLight, normal);

        float specFactor = pow(max(dot(viewDirection, reflectDirection), 0.f), 32);
        outSpecular = inPointLight.specular * specFactor;
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
