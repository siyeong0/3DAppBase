#include "Common.hlsli"

Texture2D g_heightTexture : register(t0);

PixelShaderInput main(VertexShaderInput input)
{
    PixelShaderInput output;
    
	output.posWorld = mul(float4(input.posModel, 1.0f), world).xyz;
	output.normalWorld = mul(float4(input.normalModel, 0.0f), worldIT).xyz;
	output.normalWorld = normalize(output.normalWorld);
	
	if (useHeightMap)
	{
		float height = g_heightTexture.SampleLevel(linearWrapSS, input.texcoord, 0).r;
		height = height * 2.0 - 1.0;
		output.posWorld += output.normalWorld * height * heightScale;
	}

	output.posProj = mul(float4(output.posWorld, 1.0), viewProj);
	output.texcoord = input.texcoord;
	output.tangentWorld = mul(float4(input.tangentModel, 0.0f), world).xyz;
	
	return output;
}
