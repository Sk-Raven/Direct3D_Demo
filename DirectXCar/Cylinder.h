#pragma once
#include <vector>
#include <string>
#include <map>
#include "Vertex.h"


// 网格数据
template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
struct MeshData
{
	std::vector<VertexType> vertexVec;	// 顶点数组
	std::vector<IndexType> indexVec;	// 索引数组

};

//插入数据
template<class VertexType>
inline void InsertVertexElement(VertexType& vertexDst, const VertexData& vertexSrc)
{
	static std::string semanticName;
	static const std::map<std::string, std::pair<size_t, size_t>> semanticSizeMap = {
		{"POSITION", std::pair<size_t, size_t>(0, 12)},
		{"NORMAL", std::pair<size_t, size_t>(12, 24)},
		{"TANGENT", std::pair<size_t, size_t>(24, 40)},
		{"COLOR", std::pair<size_t, size_t>(40, 56)},
		{"TEXCOORD", std::pair<size_t, size_t>(56, 64)}
	};

	for (size_t i = 0; i < ARRAYSIZE(VertexType::inputLayout); i++)
	{
		semanticName = VertexType::inputLayout[i].SemanticName;
		const auto& range = semanticSizeMap.at(semanticName);
		memcpy_s(reinterpret_cast<char*>(&vertexDst) + VertexType::inputLayout[i].AlignedByteOffset,
			range.second - range.first,
			reinterpret_cast<const char*>(&vertexSrc) + range.first,
			range.second - range.first);
	}
}

// 创建车轮子的圆柱体
template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
MeshData<VertexType, IndexType> CreateWheel(float x, float y, float z, float radius = 0.5f, float height = 1.0f, UINT slices = 20,
	const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f })
{
	MeshData<VertexType, IndexType> meshData;
	using namespace DirectX;
	int vertexCount = (4 * (slices + 1) + 2);
	int indexCount = (12 * slices);
	meshData.vertexVec.resize(vertexCount);
	meshData.indexVec.resize(indexCount);

	float h2 = height / 2;
	float theta = 0.0f;
	float per_theta = XM_2PI / slices;
	VertexData vertexData;
	UINT vIndex = 0;
	UINT iIndex = 0;
	theta = 0.0f;
	per_theta = XM_2PI / slices;
	IndexType offset = 2 * (slices + 1);
	// 放入侧面顶端点
	for (UINT i = 0; i <= slices; ++i)
	{
		theta = i * per_theta;
		vertexData = { XMFLOAT3(x + radius * cosf(theta), y + radius * sinf(theta),z + h2), XMFLOAT3(cosf(theta), 0.0f, sinf(theta)),
			XMFLOAT4(-sinf(theta), 0.0f, cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
		InsertVertexElement(meshData.vertexVec[vIndex], vertexData);
		vIndex++;
	}

	// 放入侧面底端点
	for (UINT i = 0; i <= slices; ++i)
	{
		theta = i * per_theta;
		vertexData = { XMFLOAT3(x + radius * cosf(theta), y + radius * sinf(theta),z - h2), XMFLOAT3(cosf(theta), 0.0f, sinf(theta)),
			XMFLOAT4(-sinf(theta), 0.0f, cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };
		InsertVertexElement(meshData.vertexVec[vIndex], vertexData);
		vIndex++;
	}



	for (UINT i = 0; i < slices; ++i)
	{
		meshData.indexVec[iIndex++] = i;
		meshData.indexVec[iIndex++] = i + 1;
		meshData.indexVec[iIndex++] = (slices + 1) + i + 1;

		meshData.indexVec[iIndex++] = (slices + 1) + i + 1;
		meshData.indexVec[iIndex++] = (slices + 1) + i;
		meshData.indexVec[iIndex++] = i;


	}


	VertexData vertexData1;
	// 放入顶端圆心
	vertexData1 = { XMFLOAT3(x + 0.0f, y + 0.0f, z + h2), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.5f, 0.5f) };
	InsertVertexElement(meshData.vertexVec[vIndex++], vertexData1);

	// 放入顶端圆上各点
	for (UINT i = 0; i <= slices; ++i)
	{
		theta = i * per_theta;
		vertexData1 = { XMFLOAT3(x + radius * cosf(theta), y + radius * sinf(theta),z + h2), XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(cosf(theta) / 2 + 0.5f, sinf(theta) / 2 + 0.5f) };
		InsertVertexElement(meshData.vertexVec[vIndex++], vertexData1);
	}

	// 放入底端圆心
	vertexData1 = { XMFLOAT3(x + 0.0f, y + 0.0f,z + h2), XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.5f, 0.5f) };
	InsertVertexElement(meshData.vertexVec[vIndex++], vertexData1);

	// 放入底部圆上各点
	for (UINT i = 0; i <= slices; ++i)
	{
		theta = i * per_theta;
		vertexData1 = { XMFLOAT3(x + radius * cosf(theta), y + radius * sinf(theta),z - h2), XMFLOAT3(0.0f, -1.0f, 0.0f),
			XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(cosf(theta) / 2 + 0.5f, sinf(theta) / 2 + 0.5f) };
		InsertVertexElement(meshData.vertexVec[vIndex++], vertexData1);
	}



	// 逐渐放入顶部三角形索引
	for (UINT i = 1; i <= slices; ++i)
	{
		meshData.indexVec[iIndex++] = offset;
		meshData.indexVec[iIndex++] = offset + i % (slices + 1) + 1;
		meshData.indexVec[iIndex++] = offset + i;
	}

	// 逐渐放入底部三角形索引
	offset += slices + 2;
	for (UINT i = 1; i <= slices; ++i)
	{
		meshData.indexVec[iIndex++] = offset;
		meshData.indexVec[iIndex++] = offset + i;
		meshData.indexVec[iIndex++] = offset + i % (slices + 1) + 1;
	}

	return meshData;
}


// 创建车身的圆柱体
template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
MeshData<VertexType, IndexType> CreateCylinder(float radius = 1.0f, float height = 4.0f, UINT slices = 20,
	const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f })
{
	using namespace DirectX;

	MeshData<VertexType, IndexType> meshData;
	int vertexCount = (4 * (slices + 1) + 2) * 2;
	int indexCount = (12 * slices) * 2;
	meshData.vertexVec.resize(vertexCount);
	meshData.indexVec.resize(indexCount);

	float h2 = height / 2;
	float theta = 0.0f;
	float per_theta = XM_2PI / slices;
	VertexData vertexData;
	UINT vIndex = 0;
	UINT iIndex = 0;
	theta = 0.0f;
	per_theta = XM_2PI / slices;
	IndexType offset = 2 * (slices + 1);

	// 放入侧面顶端点
	for (UINT i = 0; i <= slices; ++i)
	{
		theta = i * per_theta;
		vertexData = { XMFLOAT3(h2, radius * sinf(theta), radius * cosf(theta)), XMFLOAT3(cosf(theta), 0.0f, sinf(theta)),
			XMFLOAT4(-sinf(theta), 0.0f, cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 0.0f) };
		InsertVertexElement(meshData.vertexVec[vIndex], vertexData);
		vIndex++;
	}

	// 放入侧面底端点
	for (UINT i = 0; i <= slices; ++i)
	{
		theta = i * per_theta;
		vertexData = { XMFLOAT3(-h2, radius * sinf(theta), radius * cosf(theta)), XMFLOAT3(cosf(theta), 0.0f, sinf(theta)),
			XMFLOAT4(-sinf(theta), 0.0f, cosf(theta), 1.0f), color, XMFLOAT2(theta / XM_2PI, 1.0f) };
		InsertVertexElement(meshData.vertexVec[vIndex], vertexData);
		vIndex++;
	}



	for (UINT i = 0; i < slices; ++i)
	{
		meshData.indexVec[iIndex++] = i;
		meshData.indexVec[iIndex++] = i + 1;
		meshData.indexVec[iIndex++] = (slices + 1) + i + 1;

		meshData.indexVec[iIndex++] = (slices + 1) + i + 1;
		meshData.indexVec[iIndex++] = (slices + 1) + i;
		meshData.indexVec[iIndex++] = i;
	}


	VertexData vertexData1;
	// 放入顶端圆心
	vertexData1 = { XMFLOAT3(h2, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.5f, 0.5f) };
	InsertVertexElement(meshData.vertexVec[vIndex++], vertexData1);

	// 放入顶端圆上各点
	for (UINT i = 0; i <= slices; ++i)
	{
		theta = i * per_theta;
		vertexData1 = { XMFLOAT3(h2, radius * sinf(theta), radius * cosf(theta)), XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(cosf(theta) / 2 + 0.5f, sinf(theta) / 2 + 0.5f) };
		InsertVertexElement(meshData.vertexVec[vIndex++], vertexData1);
	}

	// 放入底端圆心
	vertexData1 = { XMFLOAT3(-h2, 0.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.5f, 0.5f) };
	InsertVertexElement(meshData.vertexVec[vIndex++], vertexData1);

	// 放入底部圆上各点
	for (UINT i = 0; i <= slices; ++i)
	{
		theta = i * per_theta;
		vertexData1 = { XMFLOAT3(-h2, radius * sinf(theta), radius * cosf(theta)), XMFLOAT3(0.0f, -1.0f, 0.0f),
			XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(cosf(theta) / 2 + 0.5f, sinf(theta) / 2 + 0.5f) };
		InsertVertexElement(meshData.vertexVec[vIndex++], vertexData1);
	}



	// 逐渐放入顶部三角形索引
	for (UINT i = 1; i <= slices; ++i)
	{
		meshData.indexVec[iIndex++] = offset;
		meshData.indexVec[iIndex++] = offset + i % (slices + 1) + 1;
		meshData.indexVec[iIndex++] = offset + i;
	}

	// 逐渐放入底部三角形索引
	offset += slices + 2;
	for (UINT i = 1; i <= slices; ++i)
	{
		meshData.indexVec[iIndex++] = offset;
		meshData.indexVec[iIndex++] = offset + i;
		meshData.indexVec[iIndex++] = offset + i % (slices + 1) + 1;
	}

	return meshData;
}

//天空盒
template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
inline MeshData<VertexType, IndexType> CreateBox(float width = 2.0f, float height = 2.0f, float depth = 2.0f,
	const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f })
{
	using namespace DirectX;

	MeshData<VertexType, IndexType> meshData;
	meshData.vertexVec.resize(24);


	VertexData vertexDataArr[24];
	float w2 = width / 2, h2 = height / 2, d2 = depth / 2;

	// 右面(+X面)
	vertexDataArr[0].pos = XMFLOAT3(w2, -h2, -d2);
	vertexDataArr[1].pos = XMFLOAT3(w2, h2, -d2);
	vertexDataArr[2].pos = XMFLOAT3(w2, h2, d2);
	vertexDataArr[3].pos = XMFLOAT3(w2, -h2, d2);
	// 左面(-X面)
	vertexDataArr[4].pos = XMFLOAT3(-w2, -h2, d2);
	vertexDataArr[5].pos = XMFLOAT3(-w2, h2, d2);
	vertexDataArr[6].pos = XMFLOAT3(-w2, h2, -d2);
	vertexDataArr[7].pos = XMFLOAT3(-w2, -h2, -d2);
	// 顶面(+Y面)
	vertexDataArr[8].pos = XMFLOAT3(-w2, h2, -d2);
	vertexDataArr[9].pos = XMFLOAT3(-w2, h2, d2);
	vertexDataArr[10].pos = XMFLOAT3(w2, h2, d2);
	vertexDataArr[11].pos = XMFLOAT3(w2, h2, -d2);
	// 底面(-Y面)
	vertexDataArr[12].pos = XMFLOAT3(w2, -h2, -d2);
	vertexDataArr[13].pos = XMFLOAT3(w2, -h2, d2);
	vertexDataArr[14].pos = XMFLOAT3(-w2, -h2, d2);
	vertexDataArr[15].pos = XMFLOAT3(-w2, -h2, -d2);
	// 背面(+Z面)
	vertexDataArr[16].pos = XMFLOAT3(w2, -h2, d2);
	vertexDataArr[17].pos = XMFLOAT3(w2, h2, d2);
	vertexDataArr[18].pos = XMFLOAT3(-w2, h2, d2);
	vertexDataArr[19].pos = XMFLOAT3(-w2, -h2, d2);
	// 正面(-Z面)
	vertexDataArr[20].pos = XMFLOAT3(-w2, -h2, -d2);
	vertexDataArr[21].pos = XMFLOAT3(-w2, h2, -d2);
	vertexDataArr[22].pos = XMFLOAT3(w2, h2, -d2);
	vertexDataArr[23].pos = XMFLOAT3(w2, -h2, -d2);

	for (UINT i = 0; i < 4; ++i)
	{
		// 右面(+X面)
		vertexDataArr[i].normal = XMFLOAT3(1.0f, 0.0f, 0.0f);
		vertexDataArr[i].tangent = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		vertexDataArr[i].color = color;
		// 左面(-X面)
		vertexDataArr[i + 4].normal = XMFLOAT3(-1.0f, 0.0f, 0.0f);
		vertexDataArr[i + 4].tangent = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);
		vertexDataArr[i + 4].color = color;
		// 顶面(+Y面)
		vertexDataArr[i + 8].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
		vertexDataArr[i + 8].tangent = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		vertexDataArr[i + 8].color = color;
		// 底面(-Y面)
		vertexDataArr[i + 12].normal = XMFLOAT3(0.0f, -1.0f, 0.0f);
		vertexDataArr[i + 12].tangent = XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f);
		vertexDataArr[i + 12].color = color;
		// 背面(+Z面)
		vertexDataArr[i + 16].normal = XMFLOAT3(0.0f, 0.0f, 1.0f);
		vertexDataArr[i + 16].tangent = XMFLOAT4(-1.0f, 0.0f, 0.0f, 1.0f);
		vertexDataArr[i + 16].color = color;
		// 正面(-Z面)
		vertexDataArr[i + 20].normal = XMFLOAT3(0.0f, 0.0f, -1.0f);
		vertexDataArr[i + 20].tangent = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		vertexDataArr[i + 20].color = color;
	}

	for (UINT i = 0; i < 6; ++i)
	{
		vertexDataArr[i * 4].tex = XMFLOAT2(0.0f, 1.0f);
		vertexDataArr[i * 4 + 1].tex = XMFLOAT2(0.0f, 0.0f);
		vertexDataArr[i * 4 + 2].tex = XMFLOAT2(1.0f, 0.0f);
		vertexDataArr[i * 4 + 3].tex = XMFLOAT2(1.0f, 1.0f);
	}

	for (UINT i = 0; i < 24; ++i)
	{
		InsertVertexElement(meshData.vertexVec[i], vertexDataArr[i]);
	}

	meshData.indexVec = {
		2, 1 ,0, 0, 3, 2,		// 右面(+X面)
		6, 5, 4, 4, 7, 6,		// 左面(-X面)
		10, 9, 8, 8, 11, 10,	// 顶面(+Y面)
		14, 13, 12, 12, 15, 14,	// 底面(-Y面)
		18, 17, 16, 16, 19, 18, // 背面(+Z面)
		22, 21, 20, 20, 23, 22	// 正面(-Z面)
	};

	return meshData;
}


//创建地板
template<class VertexType = VertexPosNormalTex, class IndexType = WORD>
MeshData<VertexType, IndexType> CreatePlane(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT2& planeSize = { 10.0f, 10.0f },
	const DirectX::XMFLOAT2& maxTexCoord = { 1.0f, 1.0f }, const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f })
{
	using namespace DirectX;

	MeshData<VertexType, IndexType> meshData;
	meshData.vertexVec.resize(4);

	VertexData vertexData;
	IndexType vIndex = 0;

	vertexData = { XMFLOAT3(center.x - planeSize.x / 2, center.y, center.z - planeSize.y / 2), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.0f,  maxTexCoord.y) };
	InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

	vertexData = { XMFLOAT3(center.x - planeSize.x / 2, center.y, center.z + planeSize.y / 2), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(0.0f, 0.0f) };
	InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

	vertexData = { XMFLOAT3(center.x + planeSize.x / 2, center.y, center.z + planeSize.y / 2), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(maxTexCoord.x, 0.0f) };
	InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

	vertexData = { XMFLOAT3(center.x + planeSize.x / 2, center.y, center.z - planeSize.y / 2), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), color, XMFLOAT2(maxTexCoord.x,  maxTexCoord.y) };
	InsertVertexElement(meshData.vertexVec[vIndex++], vertexData);

	meshData.indexVec = { 0, 1, 2, 2, 3, 0 };
	return meshData;
}





