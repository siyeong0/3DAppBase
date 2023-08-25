#pragma once
#include <vector>
#include <string>

#include "MathLib.h"
#include "Vertex.h"

struct MeshData
{
    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;
    std::string AlbedoTextureFilename;
    std::string EmissiveTextureFilename;
    std::string NormalTextureFilename;
    std::string HeightTextureFilename;
    std::string AoTextureFilename;
    std::string MetallicTextureFilename;
    std::string RoughnessTextureFilename;
    Vector3 Extends;
};
