#ifndef __COMMON_HLSLI__
#define __COMMON_HLSLI__

#define MAX_LIGHTS 3
#define LIGHT_OFF 0x00
#define LIGHT_DIRECTIONAL 0x01
#define LIGHT_POINT 0x02
#define LIGHT_SPOT 0x04
#define LIGHT_SHADOW 0x10

struct Light
{
    float3 radiance;
    float fallOffStart;
    float3 direction;
    float fallOffEnd;
    float3 position;
    float spotPower;
	
	uint type;
	float radius;
	float haloRadius;
	float haloStrength;

	matrix viewProj;
	matrix invProj;
};

struct VertexShaderInput
{
    float3 posModel : POSITION;
    float3 normalModel : NORMAL0; 
    float2 texcoord : TEXCOORD0;
    float3 tangentModel : TANGENT0;
};

struct PixelShaderInput
{
    float4 posProj : SV_POSITION; // Screen position
    float3 posWorld : POSITION; // World position (for Lighting)
    float3 normalWorld : NORMAL0;
    float2 texcoord : TEXCOORD0;
    float3 tangentWorld : TANGENT0;
    float3 color : COLOR; // for Normal lines shaders
};

cbuffer GlobalConstants : register(b0)
{
	matrix view;
	matrix proj;
	matrix viewProj;
	matrix invProj;
	matrix invView;
	matrix invViewProj; // Proj -> World

	float3 eyeWorld;
	float strengthIBL;
	
	float envLodBias = 0.0f; // Env map LodBias
	float lodBias = 2.0f;
	float globalTime;
	float dummy1;
    
	Light lights[MAX_LIGHTS];
};

cbuffer MeshConstants : register(b1)
{
	matrix world;
	matrix worldIT;	// Inverse Transpose
	matrix worldInv;
	int useHeightMap;
	float heightScale;
	float windTrunk;
	float windLeaves;
};

cbuffer MaterialConstants : register(b2)
{
	float3 albedoFactor; // baseColor
	float roughnessFactor;
	float metallicFactor;
	float3 emissionFactor;

	int useAlbedoMap;
	int useNormalMap;
	int useAOMap; // Ambient Occlusion
	int invertNormalMapY;
	int useMetallicMap;
	int useRoughnessMap;
	int useEmissiveMap;
	float dummy2;
};

// Textures
TextureCube envIBLTex : register(t10);
TextureCube specularIBLTex : register(t11);
TextureCube irradianceIBLTex : register(t12);
Texture2D brdfIBLTex : register(t13);

Texture2D depthMap : register(t14);

Texture2D shadowMaps[MAX_LIGHTS] : register(t15);

// Samplers
SamplerState linearWrapSS : register(s0);
SamplerState linearClampSS : register(s1);
SamplerState shadowPointSS : register(s2);
SamplerComparisonState shadowCompareSS : register(s3);
SamplerState pointWrapSS : register(s4);
SamplerState linearMirrorSS : register(s5);
SamplerState pointClampSS : register(s6);

#endif // __COMMON_HLSLI__