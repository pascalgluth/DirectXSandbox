struct PS_INPUT
{
    float4 position : SV_POSITION;
    float3 textureCoord : TEXCOORD;
    float3 worldPosition : WORLD_POSITION;
    float3 normal : NORMAL;
};

SamplerState objSamplerState;
TextureCube skyCubeMap;

float4 main(PS_INPUT input) : SV_TARGET
{
    return skyCubeMap.Sample(objSamplerState, input.textureCoord);
}