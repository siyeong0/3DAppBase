#include "Common.hlsli"

struct SkyboxPixelShaderInput
{
    float4 posProj : SV_POSITION;
    float3 posModel : POSITION;
};

struct PixelShaderOutput
{
    float4 pixelColor : SV_Target0;
    //float4 indexColor : SV_Target1;
};

PixelShaderOutput main(SkyboxPixelShaderInput input)
{
    PixelShaderOutput output;
	output.pixelColor = envIBLTex.SampleLevel(linearWrapSS, input.posModel.xyz, envLodBias);
	output.pixelColor.rgb *= strengthIBL;
	output.pixelColor.a = 1.0;
    
    //output.indexColor = float4(0.0, 0.0, 1.0, 0.0);
    
    return output;
}