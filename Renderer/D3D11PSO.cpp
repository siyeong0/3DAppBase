#include "pch.h"
#include "D3D11PSO.h"

void GraphicsPSO::operator=(const GraphicsPSO& pso)
{
    VertexShader = pso.VertexShader;
    PixelShader = pso.PixelShader;
    HullShader = pso.HullShader;
    DomainShader = pso.DomainShader;
    GeometryShader = pso.GeometryShader;
    InputLayout = pso.InputLayout;
    BlendState = pso.BlendState;
    DepthStencilState = pso.DepthStencilState;
    RasterizerState = pso.RasterizerState;
    StencilRef = pso.StencilRef;
    SetBlendFactor(pso.BlendFactor);
    PrimitiveTopology = pso.PrimitiveTopology;
}

void GraphicsPSO::SetBlendFactor(const float blendFactor[4]) {
    memcpy(BlendFactor, blendFactor, sizeof(float) * 4);
}