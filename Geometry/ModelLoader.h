#pragma once
#include "dllAPI.h"

#include <iostream>
#include <string>
#include <vector>
// vcpkg install assimp:x64-windows
// Preprocessor definitions -> NOMINMAX 
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "MeshData.h"
#include "Vertex.h"

class GEOMETRY_API ModelLoader
{
public:
	void Load(std::string basePath, std::string filename);
	void ProcessNode(aiNode* node, const aiScene* scene, DirectX::SimpleMath::Matrix tr);
	MeshData ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::string ReadTextureFilename(const aiScene* scene, aiMaterial* material, aiTextureType type);
	void UpdateTangents();

public:
	std::string BasePath;
	std::vector<MeshData> Meshes;
};