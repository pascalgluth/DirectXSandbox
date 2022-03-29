#pragma pack_matrix(row_major)

cbuffer globalCBuffer : register(b0)
{
    float4x4 viewProjectionMatrix;
    float3 eyePos;
    float pad;
}

cbuffer objectCBuffer : register(b1)
{
    float4x4 worldMatrix;
}

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 textureCoord : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 textureCoord : TEXCOORD;
    float3 worldPosition : WORLD_POSITION;
    float3 normal : NORMAL;
    float3 eyePos : EYEPOS;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    output.position = mul(float4(input.position, 1.0), mul(worldMatrix, viewProjectionMatrix));
    output.textureCoord = input.textureCoord;
    output.worldPosition = mul(float4(input.position, 1.f), worldMatrix);
    output.normal = normalize(mul(float4(input.normal, 0.f), worldMatrix));
    output.eyePos = float4(eyePos, 1.f);
    return output;
}