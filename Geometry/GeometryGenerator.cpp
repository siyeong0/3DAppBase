#include "pch.h" 
#include "GeometryGenerator.h"

#include "ModelLoader.h"

#include <algorithm>

static Vector3 GetMaxPos(std::vector<Vertex> vertices)
{
	Vector3 maxVec{ 0,0,0 };
	for (auto& vertex : vertices)
	{
		Vector3 v = vertex.Position;
		float ax = abs(v.x);
		float ay = abs(v.y);
		float az = abs(v.z);

		maxVec.x = std::max(maxVec.x, ax);
		maxVec.y = std::max(maxVec.y, ay);
		maxVec.z = std::max(maxVec.z, az);
	}

	return maxVec;
}

MeshData GeometryGenerator::MakeSquare(const float scale, const Vector2 texScale)
{
	std::vector<Vector3> positions;
	std::vector<Vector3> colors;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;

	// Front
	positions.push_back(Vector3(-1.0f, 1.0f, 0.0f) * scale);
	positions.push_back(Vector3(1.0f, 1.0f, 0.0f) * scale);
	positions.push_back(Vector3(1.0f, -1.0f, 0.0f) * scale);
	positions.push_back(Vector3(-1.0f, -1.0f, 0.0f) * scale);
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));

	// Texture Coordinates (Direct3D 9)
	// https://learn.microsoft.com/en-us/windows/win32/direct3d9/texture-coordinates
	texcoords.push_back(Vector2(0.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 1.0f));
	texcoords.push_back(Vector2(0.0f, 1.0f));

	MeshData meshData;

	for (size_t i = 0; i < positions.size(); i++)
	{
		Vertex v;
		v.Position = positions[i];
		v.Normal = normals[i];
		v.Texcoord = texcoords[i] * texScale;
		v.Tangent = Vector3(1.0f, 0.0f, 0.0f);

		meshData.Vertices.push_back(v);
	}
	meshData.Indices =
	{
		0, 1, 2, 0, 2, 3, // Front
	};

	meshData.Extends = Vector3(scale, scale, scale);

	return meshData;
}

MeshData GeometryGenerator::MakeSquareGrid(const int numSlices, const int numStacks, const float scale, const Vector2 texScale)
{
	MeshData meshData;

	float dx = 2.0f / numSlices;
	float dy = 2.0f / numStacks;

	float y = 1.0f;
	for (int j = 0; j < numStacks + 1; j++)
	{
		float x = -1.0f;
		for (int i = 0; i < numSlices + 1; i++)
		{
			Vertex v;
			v.Position = Vector3(x, y, 0.0f) * scale;
			v.Normal = Vector3(0.0f, 0.0f, -1.0f);
			v.Texcoord = Vector2(x + 1.0f, y + 1.0f) * 0.5f * texScale;
			v.Tangent = Vector3(1.0f, 0.0f, 0.0f);

			meshData.Vertices.push_back(v);

			x += dx;
		}
		y -= dy;
	}

	for (int j = 0; j < numStacks; j++)
	{
		for (int i = 0; i < numSlices; i++)
		{
			meshData.Indices.push_back((numSlices + 1) * j + i);
			meshData.Indices.push_back((numSlices + 1) * j + i + 1);
			meshData.Indices.push_back((numSlices + 1) * (j + 1) + i);
			meshData.Indices.push_back((numSlices + 1) * (j + 1) + i);
			meshData.Indices.push_back((numSlices + 1) * j + i + 1);
			meshData.Indices.push_back((numSlices + 1) * (j + 1) + i + 1);
		}
	}

	meshData.Extends = Vector3(scale, scale, scale);

	return meshData;
}

MeshData GeometryGenerator::MakeBox(const float scale)
{

	std::vector<Vector3> positions;
	std::vector<Vector3> colors;
	std::vector<Vector3> normals;
	std::vector<Vector2> texcoords;

	// Top
	positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
	positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
	positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
	positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
	colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, 1.0f, 0.0f));
	texcoords.push_back(Vector2(0.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 1.0f));
	texcoords.push_back(Vector2(0.0f, 1.0f));

	// Bottom
	positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
	positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
	positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
	positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	normals.push_back(Vector3(0.0f, -1.0f, 0.0f));
	texcoords.push_back(Vector2(0.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 1.0f));
	texcoords.push_back(Vector2(0.0f, 1.0f));

	// Front
	positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
	positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
	positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
	positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, -1.0f));
	texcoords.push_back(Vector2(0.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 1.0f));
	texcoords.push_back(Vector2(0.0f, 1.0f));

	// Back
	positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
	positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
	positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
	positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	colors.push_back(Vector3(0.0f, 1.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(0.0f, 0.0f, 1.0f));
	texcoords.push_back(Vector2(0.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 1.0f));
	texcoords.push_back(Vector2(0.0f, 1.0f));

	// Left
	positions.push_back(Vector3(-1.0f, -1.0f, 1.0f) * scale);
	positions.push_back(Vector3(-1.0f, 1.0f, 1.0f) * scale);
	positions.push_back(Vector3(-1.0f, 1.0f, -1.0f) * scale);
	positions.push_back(Vector3(-1.0f, -1.0f, -1.0f) * scale);
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	colors.push_back(Vector3(1.0f, 1.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(-1.0f, 0.0f, 0.0f));
	texcoords.push_back(Vector2(0.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 1.0f));
	texcoords.push_back(Vector2(0.0f, 1.0f));

	// Right
	positions.push_back(Vector3(1.0f, -1.0f, 1.0f) * scale);
	positions.push_back(Vector3(1.0f, -1.0f, -1.0f) * scale);
	positions.push_back(Vector3(1.0f, 1.0f, -1.0f) * scale);
	positions.push_back(Vector3(1.0f, 1.0f, 1.0f) * scale);
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	colors.push_back(Vector3(1.0f, 0.0f, 1.0f));
	normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
	normals.push_back(Vector3(1.0f, 0.0f, 0.0f));
	texcoords.push_back(Vector2(0.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 0.0f));
	texcoords.push_back(Vector2(1.0f, 1.0f));
	texcoords.push_back(Vector2(0.0f, 1.0f));

	MeshData meshData;
	for (size_t i = 0; i < positions.size(); i++)
	{
		Vertex v;
		v.Position = positions[i];
		v.Normal = normals[i];
		v.Texcoord = texcoords[i];
		meshData.Vertices.push_back(v);
	}

	meshData.Indices =
	{
		0,  1,  2,  0,  2,  3,  // Top
		4,  5,  6,  4,  6,  7,  // Bottom
		8,  9,  10, 8,  10, 11, // Front
		12, 13, 14, 12, 14, 15, // Back
		16, 17, 18, 16, 18, 19, // Left
		20, 21, 22, 20, 22, 23  // Right
	};

	meshData.Extends = Vector3(scale, scale, scale);

	return meshData;
}

MeshData GeometryGenerator::MakeCylinder(const float bottomRadius, const float topRadius, float height, int numSlices)
{
	const float dTheta = -_2PI / float(numSlices);

	MeshData meshData;

	std::vector<Vertex>& vertices = meshData.Vertices;

	for (int i = 0; i <= numSlices; i++)
	{
		Vertex v;
		v.Position = Vector3::Transform(Vector3(bottomRadius, -0.5f * height, 0.0f), Matrix::CreateRotationY(dTheta * float(i)));
		v.Normal = v.Position - Vector3(0.0f, -0.5f * height, 0.0f);
		v.Normal.Normalize();
		v.Texcoord = Vector2(float(i) / numSlices, 1.0f);

		vertices.push_back(v);
	}

	for (int i = 0; i <= numSlices; i++)
	{
		Vertex v;
		v.Position = Vector3::Transform(Vector3(topRadius, 0.5f * height, 0.0f), Matrix::CreateRotationY(dTheta * float(i)));
		v.Normal = v.Position - Vector3(0.0f, 0.5f * height, 0.0f);
		v.Normal.Normalize();
		v.Texcoord = Vector2(float(i) / numSlices, 0.0f);

		vertices.push_back(v);
	}

	std::vector<uint32_t>& indices = meshData.Indices;

	for (int i = 0; i < numSlices; i++)
	{
		indices.push_back(i);
		indices.push_back(i + numSlices + 1);
		indices.push_back(i + 1 + numSlices + 1);

		indices.push_back(i);
		indices.push_back(i + 1 + numSlices + 1);
		indices.push_back(i + 1);
	}

	meshData.Extends = Vector3(std::max(bottomRadius, topRadius), height, std::max(bottomRadius, topRadius));

	return meshData;
}

MeshData GeometryGenerator::MakeSphere(const float radius, const int numSlices, const int numStacks, const Vector2 texScale)
{

	// Ref: OpenGL Sphere
	// http://www.songho.ca/opengl/gl_sphere.html

	const float dTheta = -_2PI / float(numSlices);
	const float dPhi = -_PI / float(numStacks);

	MeshData meshData;

	std::vector<Vertex>& vertices = meshData.Vertices;

	for (int j = 0; j <= numStacks; j++)
	{
		Vector3 stackStartPoint = Vector3::Transform(Vector3(0.0f, -radius, 0.0f), Matrix::CreateRotationZ(dPhi * j));

		for (int i = 0; i <= numSlices; i++)
		{
			Vertex v;

			v.Position = Vector3::Transform(stackStartPoint, Matrix::CreateRotationY(dTheta * float(i)));

			v.Normal = v.Position;
			v.Normal.Normalize();
			v.Texcoord = Vector2(float(i) / numSlices, 1.0f - float(j) / numStacks) * texScale;

			Vector3 biTangent = Vector3(0.0f, 1.0f, 0.0f);
			Vector3 normalOrth = v.Normal - biTangent.Dot(v.Normal) * v.Normal;
			normalOrth.Normalize();

			v.Tangent = biTangent.Cross(normalOrth);
			v.Tangent.Normalize();

			vertices.push_back(v);
		}
	}

	std::vector<uint32_t>& indices = meshData.Indices;

	for (int j = 0; j < numStacks; j++)
	{
		const int offset = (numSlices + 1) * j;
		for (int i = 0; i < numSlices; i++)
		{
			indices.push_back(offset + i);
			indices.push_back(offset + i + numSlices + 1);
			indices.push_back(offset + i + 1 + numSlices + 1);

			indices.push_back(offset + i);
			indices.push_back(offset + i + 1 + numSlices + 1);
			indices.push_back(offset + i + 1);
		}
	}

	meshData.Extends = Vector3(radius, radius, radius);

	return meshData;
}

MeshData GeometryGenerator::MakeIcosahedron()
{
	const float X = 0.525731f;
	const float Z = 0.850651f;

	MeshData newMesh;

	std::vector<Vector3> pos =
	{
		Vector3(-X, 0.0f, Z), Vector3(X, 0.0f, Z),   Vector3(-X, 0.0f, -Z),
		Vector3(X, 0.0f, -Z), Vector3(0.0f, Z, X),   Vector3(0.0f, Z, -X),
		Vector3(0.0f, -Z, X), Vector3(0.0f, -Z, -X), Vector3(Z, X, 0.0f),
		Vector3(-Z, X, 0.0f), Vector3(Z, -X, 0.0f),  Vector3(-Z, -X, 0.0f)
	};

	for (size_t i = 0; i < pos.size(); i++)
	{
		Vertex v;
		v.Position = pos[i];
		v.Normal = v.Position;
		v.Normal.Normalize();

		newMesh.Vertices.push_back(v);
	}

	newMesh.Indices =
	{
		1,  4,  0, 4,  9, 0, 4, 5,  9, 8, 5, 4,  1,  8, 4,
		1,  10, 8, 10, 3, 8, 8, 3,  5, 3, 2, 5,  3,  7, 2,
		3,  10, 7, 10, 6, 7, 6, 11, 7, 6, 0, 11, 6,  1, 0,
		10, 1,  6, 11, 0, 9, 2, 11, 9, 5, 2, 9,  11, 2, 7
	};

	newMesh.Extends = GetMaxPos(newMesh.Vertices);

	return newMesh;
}

MeshData GeometryGenerator::MakeTetrahedron()
{

	// Regular Tetrahedron
	// https://mathworld.wolfram.com/RegularTetrahedron.html

	const float a = 1.0f;
	const float x = sqrt(3.0f) / 3.0f * a;
	const float d = sqrt(3.0f) / 6.0f * a; // = x / 2
	const float h = sqrt(6.0f) / 3.0f * a;

	std::vector<Vector3> points =
	{
		{0.0f, x, 0.0f},
		{-0.5f * a, -d, 0.0f},
		{+0.5f * a, -d, 0.0f},
		{0.0f, 0.0f, h}
	};

	Vector3 center = Vector3(0.0f);

	for (int i = 0; i < 4; i++)
	{
		center += points[i];
	}
	center /= 4.0f;

	for (int i = 0; i < 4; i++)
	{
		points[i] -= center;
	}

	MeshData meshData;

	for (int i = 0; i < points.size(); i++)
	{

		Vertex v;
		v.Position = points[i];
		v.Normal = v.Position;
		v.Normal.Normalize();

		meshData.Vertices.push_back(v);
	}

	meshData.Indices = { 0, 1, 2, 3, 2, 1, 0, 3, 1, 0, 2, 3 };

	meshData.Extends = GetMaxPos(meshData.Vertices);

	return meshData;
}
MeshData GeometryGenerator::SubdivideToSphere(const float radius, MeshData meshData)
{
	using namespace DirectX;
	using DirectX::SimpleMath::Matrix;
	using DirectX::SimpleMath::Vector3;

	// Assert center == (0,0,0)
	for (auto& v : meshData.Vertices)
	{
		v.Position = v.Normal * radius;
	}

	auto ProjectVertex = [&](Vertex& v) {
		v.Normal = v.Position;
		v.Normal.Normalize();
		v.Position = v.Normal * radius;

		// atan vs atan2
		// https://stackoverflow.com/questions/283406/what-is-the-difference-between-atan-and-atan2-in-c
		// const float theta = atan2f(v.position.z, v.position.x);
		// const float phi = acosf(v.position.y / radius);
		// v.texcoord.x = theta / XM_2PI;
		// v.texcoord.y = phi / XM_PI;
		};

	auto UpdateFaceNormal = [](Vertex& v0, Vertex& v1, Vertex& v2)
		{
			auto faceNormal = (v1.Position - v0.Position).Cross(v2.Position - v0.Position);
			faceNormal.Normalize();
			v0.Normal = faceNormal;
			v1.Normal = faceNormal;
			v2.Normal = faceNormal;
		};

	MeshData newMesh;
	uint32_t count = 0;
	for (size_t i = 0; i < meshData.Indices.size(); i += 3)
	{
		size_t i0 = meshData.Indices[i];
		size_t i1 = meshData.Indices[i + 1];
		size_t i2 = meshData.Indices[i + 2];

		Vertex v0 = meshData.Vertices[i0];
		Vertex v1 = meshData.Vertices[i1];
		Vertex v2 = meshData.Vertices[i2];

		Vertex v3;
		v3.Position = (v0.Position + v2.Position) * 0.5f;
		v3.Texcoord = (v0.Texcoord + v2.Texcoord) * 0.5f;
		ProjectVertex(v3);

		Vertex v4;
		v4.Position = (v0.Position + v1.Position) * 0.5f;
		v4.Texcoord = (v0.Texcoord + v1.Texcoord) * 0.5f;
		ProjectVertex(v4);

		Vertex v5;
		v5.Position = (v1.Position + v2.Position) * 0.5f;
		v5.Texcoord = (v1.Texcoord + v2.Texcoord) * 0.5f;
		ProjectVertex(v5);

		// UpdateFaceNormal(v4, v1, v5);
		// UpdateFaceNormal(v0, v4, v3);
		// UpdateFaceNormal(v3, v4, v5);
		// UpdateFaceNormal(v3, v5, v2);

		newMesh.Vertices.push_back(v4);
		newMesh.Vertices.push_back(v1);
		newMesh.Vertices.push_back(v5);

		newMesh.Vertices.push_back(v0);
		newMesh.Vertices.push_back(v4);
		newMesh.Vertices.push_back(v3);

		newMesh.Vertices.push_back(v3);
		newMesh.Vertices.push_back(v4);
		newMesh.Vertices.push_back(v5);

		newMesh.Vertices.push_back(v3);
		newMesh.Vertices.push_back(v5);
		newMesh.Vertices.push_back(v2);

		for (uint32_t j = 0; j < 12; j++)
		{
			newMesh.Indices.push_back(j + count);
		}
		count += 12;
	}

	meshData.Extends = GetMaxPos(meshData.Vertices);

	return newMesh;
}
std::vector<MeshData> GeometryGenerator::ReadFromFile(std::string basePath, std::string filename)
{
	using namespace DirectX;

	ModelLoader modelLoader;
	modelLoader.Load(basePath, filename);
	std::vector<MeshData>& meshes = modelLoader.Meshes;

	// Normalize vertices
	Vector3 vmin(1000, 1000, 1000);
	Vector3 vmax(-1000, -1000, -1000);
	for (auto& mesh : meshes)
	{
		for (auto& v : mesh.Vertices)
		{
			vmin.x = XMMin(vmin.x, v.Position.x);
			vmin.y = XMMin(vmin.y, v.Position.y);
			vmin.z = XMMin(vmin.z, v.Position.z);
			vmax.x = XMMax(vmax.x, v.Position.x);
			vmax.y = XMMax(vmax.y, v.Position.y);
			vmax.z = XMMax(vmax.z, v.Position.z);
		}
	}

	float dx = vmax.x - vmin.x, dy = vmax.y - vmin.y, dz = vmax.z - vmin.z;
	float dl = XMMax(XMMax(dx, dy), dz);
	float cx = (vmax.x + vmin.x) * 0.5f, cy = (vmax.y + vmin.y) * 0.5f, cz = (vmax.z + vmin.z) * 0.5f;

	for (auto& mesh : meshes)
	{
		for (auto& v : mesh.Vertices)
		{
			v.Position.x = (v.Position.x - cx) / dl;
			v.Position.y = (v.Position.y - cy) / dl;
			v.Position.z = (v.Position.z - cz) / dl;
		}
	}

	for (auto& mesh : meshes)
	{
		mesh.Extends = GetMaxPos(mesh.Vertices);
	}

	return meshes;
}