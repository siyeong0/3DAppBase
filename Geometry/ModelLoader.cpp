#include "pch.h" 
#include "ModelLoader.h"

#include <filesystem>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <io.h>
#include <regex>

using namespace std;
using namespace DirectX::SimpleMath;

void ModelLoader::Load(std::string basePath, std::string filename)
{
	this->BasePath = basePath;

	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(this->BasePath + filename,
		aiProcess_Triangulate | aiProcess_ConvertToLeftHanded);

	if (!pScene)
	{
		std::cout << "Failed to read file: " << this->BasePath + filename << std::endl;
	}
	else
	{
		Matrix tr; // Initial transformation
		ProcessNode(pScene->mRootNode, pScene, tr);
	}

	UpdateTangents();
}

void ModelLoader::UpdateTangents() {

	using namespace std;
	using namespace DirectX;

	// https://github.com/microsoft/DirectXMesh/wiki/ComputeTangentFrame

	for (auto& m : this->Meshes)
	{

		vector<Vector3> positions(m.Vertices.size());
		vector<Vector3> normals(m.Vertices.size());
		vector<Vector2> texcoords(m.Vertices.size());
		vector<Vector3> tangents(m.Vertices.size());
		vector<Vector3> bitangents(m.Vertices.size());

		for (size_t i = 0; i < m.Vertices.size(); i++)
		{
			auto& v = m.Vertices[i];
			positions[i] = v.Position;
			normals[i] = v.Normal;
			texcoords[i] = v.Texcoord;
		}

		ComputeTangentFrame(m.Indices.data(), m.Indices.size() / 3,
			positions.data(), normals.data(), texcoords.data(),
			m.Vertices.size(), tangents.data(),
			bitangents.data());

		for (size_t i = 0; i < m.Vertices.size(); i++)
		{
			m.Vertices[i].Tangent = tangents[i];
		}
	}
}

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, Matrix tr)
{
	Matrix m;
	ai_real* temp = &node->mTransformation.a1;
	float* mTemp = &m._11;
	for (int t = 0; t < 16; t++)
	{
		mTemp[t] = float(temp[t]);
	}
	m = m.Transpose() * tr;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{

		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		auto newMesh = this->ProcessMesh(mesh, scene);

		for (auto& v : newMesh.Vertices)
		{
			v.Position = DirectX::SimpleMath::Vector3::Transform(v.Position, m);
		}

		Meshes.push_back(newMesh);
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene, m);
	}
}

string ModelLoader::ReadTextureFilename(const aiScene* scene, aiMaterial* material, aiTextureType type)
{
	if (material->GetTextureCount(type) > 0)
	{
		aiString filepath;
		material->GetTexture(type, 0, &filepath);
		string fullPath = BasePath + string(filesystem::path(filepath.C_Str()).filename().string());

		if (!filesystem::exists(fullPath))
		{
			const aiTexture* texture = scene->GetEmbeddedTexture(filepath.C_Str());
			if (texture)
			{
				if (string(texture->achFormatHint).find("png") != string::npos)
				{
					ofstream fs(fullPath.c_str(), ios::binary | ios::out);
					fs.write((char*)texture->pcData, texture->mWidth);
					fs.close();
				}
			}
			else
			{
				cout << fullPath << " doesn't exists. Return empty filename."
					<< endl;
			}
		}
		else
		{
			return fullPath;
		}

		return fullPath;
	}
	else
	{
		return "";
	}
}

MeshData ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	// Data to fill
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	// Walk through each of the mesh's vertices
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;

		vertex.Position.x = mesh->mVertices[i].x;
		vertex.Position.y = mesh->mVertices[i].y;
		vertex.Position.z = mesh->mVertices[i].z;

		vertex.Normal.x = mesh->mNormals[i].x;
		//vertex.normalModel.y = mesh->mNormals[i].y;
		//vertex.normalModel.z = mesh->mNormals[i].z;
		vertex.Normal.y = mesh->mNormals[i].z;
		vertex.Normal.z = -mesh->mNormals[i].y;
		vertex.Normal.Normalize();

		if (mesh->mTextureCoords[0])
		{
			vertex.Texcoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.Texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	MeshData newMesh;
	newMesh.Vertices = vertices;
	newMesh.Indices = indices;

	// http://assimp.sourceforge.net/lib_html/materials.html
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		newMesh.AlbedoTextureFilename = ReadTextureFilename(scene, material, aiTextureType_BASE_COLOR);
		if (newMesh.AlbedoTextureFilename.empty())
		{
			newMesh.AlbedoTextureFilename = ReadTextureFilename(scene, material, aiTextureType_DIFFUSE);
		}
		newMesh.EmissiveTextureFilename = ReadTextureFilename(scene, material, aiTextureType_EMISSIVE);
		newMesh.HeightTextureFilename = ReadTextureFilename(scene, material, aiTextureType_HEIGHT);
		newMesh.NormalTextureFilename = ReadTextureFilename(scene, material, aiTextureType_NORMALS);
		newMesh.MetallicTextureFilename = ReadTextureFilename(scene, material, aiTextureType_METALNESS);
		newMesh.RoughnessTextureFilename = ReadTextureFilename(scene, material, aiTextureType_DIFFUSE_ROUGHNESS);
		newMesh.AoTextureFilename = ReadTextureFilename(scene, material, aiTextureType_AMBIENT_OCCLUSION);
		if (newMesh.AoTextureFilename.empty())
		{
			newMesh.AoTextureFilename = ReadTextureFilename(scene, material, aiTextureType_LIGHTMAP);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		vector<string> fileList;
		for (auto& file : std::filesystem::directory_iterator(BasePath))
		{
			fileList.push_back(file.path().string());
		}

		auto findTexIfEmpty = [](string& texFileName, const regex& reg, const vector<string> fileList)
			{
				if (texFileName.empty())
				{
					for (const auto& file : fileList)
					{
						if (std::regex_match(file, reg))
						{
							texFileName = file;
							break;
						}
					}
				}
			};
		findTexIfEmpty(newMesh.AlbedoTextureFilename, regex("^.*albedo.(?:jpg|png)$"), fileList);
		findTexIfEmpty(newMesh.EmissiveTextureFilename, regex("^.*emissive.(?:jpg|png)$"), fileList);
		findTexIfEmpty(newMesh.HeightTextureFilename, regex("^.*height.(?:jpg|png)$"), fileList);
		findTexIfEmpty(newMesh.NormalTextureFilename, regex("^.*normal.(?:jpg|png)$"), fileList);
		findTexIfEmpty(newMesh.MetallicTextureFilename, regex("^.*metallic.(?:jpg|png)$"), fileList);
		findTexIfEmpty(newMesh.RoughnessTextureFilename, regex("^.*roughness.(?:jpg|png)$"), fileList);
		findTexIfEmpty(newMesh.AoTextureFilename, regex("^.*ao.(?:jpg|png)$"), fileList);
	}

	return newMesh;
}
