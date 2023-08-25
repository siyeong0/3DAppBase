#include "Common.hlsli" 

struct PixelShaderOutput
{
	float4 pixelColor : SV_Target0;
};

PixelShaderOutput main(PixelShaderInput input)
{
	PixelShaderOutput output;
	output.pixelColor.rgba = 1.0;
    
	return output;
}
