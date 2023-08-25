#pragma once
#include <vector>

#include "MathLib.h"
#include "Vertex.h"

struct MeshData
{
    std::vector<Vertex> Vertices;
    vector<uint32_t> Indices;
    string AlbedoTextureFilename;
    string EmissiveTextureFilename;
    string NormalTextureFilename;
    string HeightTextureFilename;
    string AoTextureFilename;
    string MetallicTextureFilename;
    string RoughnessTextureFilename;
    Vector3 Extends;
};
