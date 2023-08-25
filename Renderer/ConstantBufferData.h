#pragma once
#include "dllAPI.h"
#include "MathLib.h"

#define MAX_LIGHTS 3
#define LIGHT_OFF 0x00
#define LIGHT_DIRECTIONAL 0x01
#define LIGHT_POINT 0x02
#define LIGHT_SPOT 0x04
#define LIGHT_SHADOW 0x10

struct RENDERER_API Light
{
    Vector3 Radiance = Vector3(0.0f);
    float FallOffStart = 0.0f;
    Vector3 Direction = Vector3(0.0f, 0.0f, 1.0f);
    float FallOffEnd = 10.0f;
    Vector3 Position = Vector3(0.0f, 0.0f, -2.0f);
    float SpotPower = 100.0f;

    // Light type bitmasking
    // ex) LIGHT_SPOT | LIGHT_SHADOW
    uint32_t Type = LIGHT_OFF;
    float Radius = 0.035f; // 반지름

    float HaloRadius = 0.0f;
    float haloStrength = 0.0f;

    Matrix ViewProj; // 그림자 렌더링에 필요
    Matrix InvProj;  // 그림자 렌더링 디버깅용
};

__declspec(align(256))
struct RENDERER_API GlobalConstantsData  // Register b0
{
    Matrix View;
    Matrix Proj;
    Matrix ViewProj;
    Matrix InvProj;
    Matrix InvView;
    Matrix InvViewProj;

    Vector3 EyeWorld;
    float IBLStrength = 0.5f;

    float EnvLodBias = 0.0f; // Envmap LodBias
    float LodBias = 2.0f;    // Other objects LodBias
    float GlobalTime = 0.0f;
    float Dummy1 = 0.0f;

    Light Lights[MAX_LIGHTS];
};

__declspec(align(256))
struct RENDERER_API MeshConstantsData  // Register b1
{
    Matrix World;
    Matrix WorldInvTranspose;
    Matrix WorldInv;
    int bUseHeightMap = 0;
    float HeightScale = 0.0f;
    float WindTrunk = 0.0f;
    float WindLeaves = 0.0f;
};

__declspec(align(256))
struct RENDERER_API MaterialConstantsData  // Register b2
{
    Vector3 albedoFactor = Vector3(1.0f);
    float roughnessFactor = 1.0f;
    float metallicFactor = 1.0f;
    Vector3 emissionFactor = Vector3(0.0f);

    // 여러 옵션들에 uint 플래그 하나만 사용할 수도 있습니다.
    int useAlbedoMap = 0;
    int useNormalMap = 0;
    int useAOMap = 0;
    int invertNormalMapY = 0;
    int useMetallicMap = 0;
    int useRoughnessMap = 0;
    int useEmissiveMap = 0;
    float dummy = 0.0f;

    // 참고 flags 구현
    /* union {
        uint32_t flags;
        struct {
            // UV0 or UV1 for each texture
            uint32_t baseColorUV : 1;
            uint32_t metallicRoughnessUV : 1;
            uint32_t occlusionUV : 1;
            uint32_t emissiveUV : 1;
            uint32_t normalUV : 1;

            // Three special modes
            uint32_t twoSided : 1;
            uint32_t alphaTest : 1;
            uint32_t alphaBlend : 1;

            uint32_t _pad : 8;

            uint32_t alphaRef : 16; // half float
        };
    };*/
};

__declspec(align(256))
struct RENDERER_API DrawNormalConstantData
{
    float Scale = 0.1f;
    float Dummy[3] = {};
};
