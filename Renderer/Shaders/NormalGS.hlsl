#include "Common.hlsli"

cbuffer NormalVertexConstantData : register(b2)
{
    float scale;
};

struct GeometryShaderInput
{
    float4 posModel : SV_POSITION;
    float3 normalModel : NORMAL;
};

struct NormalPixelShaderInput
{
    float4 pos : SV_POSITION;
    float3 color : COLOR;
};

[maxvertexcount(2)]
void main(point GeometryShaderInput input[1], inout LineStream<NormalPixelShaderInput> outputStream)
{
	NormalPixelShaderInput output;
    
    float4 posWorld = mul(input[0].posModel, world);
    float4 normalModel = float4(input[0].normalModel, 0.0);
    float4 normalWorld = mul(normalModel, worldIT);
    normalWorld = float4(normalize(normalWorld.xyz), 0.0);
    
    output.pos = mul(posWorld, view);
    output.pos = mul(output.pos, proj);
    output.color = float3(1.0, 1.0, 0.0);
    outputStream.Append(output);
    
    output.pos = mul(posWorld + 0.1 * scale * normalWorld, view);
	output.pos = mul(output.pos, proj);
    output.color = float3(1.0, 0.0, 0.0);
    outputStream.Append(output);
}
