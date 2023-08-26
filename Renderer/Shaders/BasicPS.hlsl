#include "Common.hlsli"
#include "DiskSamples.hlsli"
// https://github.com/Nadrin/PBR/blob/master/data/shaders/hlsl/pbr.hlsl

Texture2D albedoTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D aoTex : register(t2);
Texture2D metallicTex : register(t3);
Texture2D roughnessTex : register(t4);
Texture2D emissiveTex : register(t5);

static const float3 Fdielectric = 0.04; // 비금속(Dielectric) 재질의 F0

struct PixelShaderOutput
{
	float4 pixelColor : SV_Target0;
};

float3 SchlickFresnel(float3 F0, float NdotH)
{
	return F0 + (1.0 - F0) * pow(2.0, (-5.55473 * NdotH - 6.98316) * NdotH);
    //return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 GetNormal(PixelShaderInput input)
{
	float3 normalWorld = input.normalWorld;
    
	if (useNormalMap)
	{
		float3 normal = normalTex.SampleLevel(linearWrapSS, input.texcoord, 0.0).rgb;
		normal = 2.0 * normal - 1.0;

        // OpenGL to Direct3D
		normal.y = invertNormalMapY ? -normal.y : normal.y;
        
		float3 N = normalWorld;
		float3 T = normalize(input.tangentWorld - dot(input.tangentWorld, N) * N);
		float3 B = cross(N, T);
        
		float3x3 TBN = float3x3(T, B, N);
		normalWorld = normalize(mul(normal, TBN));
	}
    
	return normalWorld;
}

float3 DiffuseIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
                  float metallic)
{
	float3 F0 = lerp(Fdielectric, albedo, metallic);
	float3 F = SchlickFresnel(F0, max(0.0, dot(normalWorld, pixelToEye)));
	float3 kd = lerp(1.0 - F, 0.0, metallic);
	float3 irradiance = irradianceIBLTex.Sample(linearWrapSS, normalWorld).rgb;
    
	return kd * albedo * irradiance;
}

float3 SpecularIBL(float3 albedo, float3 normalWorld, float3 pixelToEye,
                   float metallic, float roughness)
{
	float2 specularBRDF = brdfIBLTex.Sample(linearClampSS, float2(dot(normalWorld, pixelToEye), 1.0 - roughness)).rg;
	float3 specularIrradiance = specularIBLTex.SampleLevel(linearWrapSS, reflect(-pixelToEye, normalWorld),
                                                            3 + roughness * 5.0f).rgb;
	const float3 Fdielectric = 0.04;
	float3 F0 = lerp(Fdielectric, albedo, metallic);

	return (F0 * specularBRDF.x + specularBRDF.y) * specularIrradiance;
}

float3 AmbientLightingByIBL(float3 albedo, float3 normalW, float3 pixelToEye, float ao,
                            float metallic, float roughness)
{
	float3 diffuseIBL = DiffuseIBL(albedo, normalW, pixelToEye, metallic);
	float3 specularIBL = SpecularIBL(albedo, normalW, pixelToEye, metallic, roughness);
    
	return (diffuseIBL + specularIBL) * ao;
}

// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2.
float NdfGGX(float NdotH, float roughness)
{
	float alpha = roughness * roughness;
	float alphaSq = alpha * alpha;
	float denom = (NdotH * NdotH) * (alphaSq - 1.0) + 1.0;

	return alphaSq / (3.141592 * denom * denom);
}

// Single term for separable Schlick-GGX below.
float SchlickG1(float NdotV, float k)
{
	return NdotV / (NdotV * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float SchlickGGX(float NdotI, float NdotO, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;
	return SchlickG1(NdotI, k) * SchlickG1(NdotO, k);
}

// NdcDepth to ViewDepth
float N2V(float ndcDepth, matrix invProj)
{
    float4 pointView = mul(float4(0, 0, ndcDepth, 1), invProj);
    return pointView.z / pointView.w;
}

// Light Constants
#define NEAR_PLANE 0.1
#define LIGHT_FRUSTUM_WIDTH 0.34641

void FindBlocker(out float avgBlockerDepthView, out float numBlockers, float2 uv,
				float zReceiverView, Texture2D shadowMap, matrix invProj, float lightRadiusWorld)
{
	float lightRadiusUV = lightRadiusWorld / LIGHT_FRUSTUM_WIDTH;
	float searchRadius = lightRadiusUV * (zReceiverView - NEAR_PLANE) / zReceiverView;
	
	float blockerSum = 0.0f;
	numBlockers = 0;
	for (int i = 0; i < 64;i++)
	{
		float shadowMapDepth = shadowMap.SampleLevel(shadowPointSS, float2(uv + diskSamples64[i] * searchRadius), 0).r;
		shadowMapDepth = N2V(shadowMapDepth, invProj);
		
		if(shadowMapDepth < zReceiverView)
		{
			blockerSum += shadowMapDepth;
			numBlockers++;
		}
	}
	avgBlockerDepthView = blockerSum / numBlockers;
}

float PCFFilter(float2 uv, float zReceiverNdc, float filterRadiusUV, Texture2D shadowMap)
{
	float sum = 0.0;
	for (int i = 0; i < 64; i++)
	{
		float2 offset = diskSamples64[i] * filterRadiusUV;
		sum += shadowMap.SampleCmpLevelZero(shadowCompareSS, uv + offset, zReceiverNdc);
	}
	return sum / 64.0;
}

float PCSS(float2 uv, float zReceiverNdc, Texture2D shadowMap, matrix invProj, float lightRadiusWorld)
{
	float lightRadiusUV = lightRadiusWorld / LIGHT_FRUSTUM_WIDTH;
	float zReceiverView = N2V(zReceiverNdc, invProj);
	
	float avgBlockerDepthView = 0;
	float numBlockers = 0;
	FindBlocker(avgBlockerDepthView, numBlockers, uv, zReceiverView, shadowMap, proj, lightRadiusWorld);
	
	if(numBlockers < 1)
	{
		return 1.0;
	}
	else
	{
		float penumbraRatio = (zReceiverView - avgBlockerDepthView) / avgBlockerDepthView;
		float filterRadiusUV = penumbraRatio * lightRadiusUV * NEAR_PLANE / zReceiverView;
		
		return PCFFilter(uv, zReceiverNdc, filterRadiusUV, shadowMap);
	}
}

float3 LightRadiance(Light light, float3 posWorld, float3 normalWorld, Texture2D shadowMap)
{
    // Directional light
	float3 lightVec = light.type & LIGHT_DIRECTIONAL ? -light.direction : light.position - posWorld;
        
	float lightDist = length(lightVec);
	lightVec /= lightDist;

    // Spot light
	float spotFator = light.type & LIGHT_SPOT ? pow(max(-dot(lightVec, light.direction), 0.0f), light.spotPower) : 1.0f;
		
    // Distance attenuation
	float att = saturate((light.fallOffEnd - lightDist) / (light.fallOffEnd - light.fallOffStart));
						 
    // Shadow map
	float shadowFactor = 1.0;
	if (light.type & LIGHT_SHADOW)
	{
		float4 lightScreen = mul(float4(posWorld, 1.0), light.viewProj);
		lightScreen.xyz /= lightScreen.w;
        
		float2 lightTexcoord = float2(lightScreen.x, lightScreen.y);
		lightTexcoord.x = (lightTexcoord.x + 1.0) / 2.0;
		lightTexcoord.y = (-lightTexcoord.y + 1.0) / 2.0;
        
		//float depth = shadowMap.Sample(shadowPointSS, lightTexcoord).r;
		
		//if (depth + 1e-3 < lightScreen.z)
		//{
		//	shadowFactor = 0.0; // <- 0.0의 의미는?
		//}
		
		uint width, height, numMips;
		shadowMap.GetDimensions(0, width, height, numMips);
		
		float dx = 5.0f / float(width);
		//shadowFactor = PCFFilter(lightTexcoord.xy, lightScreen.z - 0.001, dx, shadowMap);
		shadowFactor = PCSS(lightTexcoord.xy, lightScreen.z - 0.001, shadowMap, light.invProj, light.radius);
	}
	
	float3 radiance = light.radiance * spotFator * att * shadowFactor;
	return radiance;
}

PixelShaderOutput main(PixelShaderInput input)
{
	float3 pixelToEye = normalize(eyeWorld - input.posWorld);
	float3 normalWorld = GetNormal(input);

	float3 albedo = useAlbedoMap ? albedoTex.Sample(linearWrapSS, input.texcoord).rgb : albedoFactor;
	float ao = useAOMap ? aoTex.SampleLevel(linearWrapSS, input.texcoord, 0.0).r : 1.0;
	float metallic = useMetallicMap ? metallicTex.Sample(linearWrapSS, input.texcoord).b : metallicFactor;
	float roughness = useRoughnessMap ? roughnessTex.Sample(linearWrapSS, input.texcoord).g : roughnessFactor;
	float3 emission = useEmissiveMap ? emissiveTex.Sample(linearWrapSS, input.texcoord).rgb : emissionFactor;
	
	float3 ambientLighting = AmbientLightingByIBL(albedo, normalWorld, pixelToEye, ao, metallic, roughness) * strengthIBL;
	float3 directLighting = float3(0, 0, 0);
	
	float3 radiance = 0.0f;
	[unroll] // warning X3550: sampler array index must be a literal expression, forcing loop to unroll
	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if (lights[i].type)
		{
			float3 lightVec = lights[i].position - input.posWorld;
			float lightDist = length(lightVec);
			lightVec /= lightDist;
			float3 halfway = normalize(pixelToEye + lightVec);
        
			float NdotI = max(0.0, dot(normalWorld, lightVec));
			float NdotH = max(0.0, dot(normalWorld, halfway));
			float NdotO = max(0.0, dot(normalWorld, pixelToEye));
        
			const float3 Fdielectric = 0.04; // 비금속(Dielectric) 재질의 F0
			float3 F0 = lerp(Fdielectric, albedo, metallic);
			float3 F = SchlickFresnel(F0, max(0.0, dot(halfway, pixelToEye)));
			float3 kd = lerp(float3(1, 1, 1) - F, float3(0, 0, 0), metallic);
			float3 diffuseBRDF = kd * albedo;

			float D = NdfGGX(NdotH, roughness);
			float3 G = SchlickGGX(NdotI, NdotO, roughness);
			float3 specularBRDF = (F * D * G) / max(1e-5, 4.0 * NdotI * NdotO);
            
			radiance = LightRadiance(lights[i], input.posWorld, normalWorld, shadowMaps[i]);
            
			directLighting += (diffuseBRDF + specularBRDF) * radiance * NdotI;
		}
	}
    
	PixelShaderOutput output;
	output.pixelColor = float4(ambientLighting + directLighting + emission, 1.0);
	output.pixelColor = clamp(output.pixelColor, 0.0, 1000.0);
	
	return output;
}
