#pragma once
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include"Cylinder.h"
#include"Light.h"
#include<math.h>
#include"Helper.h"

struct Light
{
	DirectionalLight dirLight[10];
	PointLight pointLight[10];
	SpotLight spotLight[10];
	Material material;
	int numDirLight;
	int numPointLight;
	int numSpotLight;
	float pad;		// 打包保证16字节对齐
};

class Object
{
protected:
	DirectX::XMFLOAT4X4 m_WorldMatrix;				    // 世界矩阵
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_pTexture;		// 纹理
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pVertexBuffer;				// 顶点缓冲区
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_pIndexBuffer;				// 索引缓冲区
	UINT m_VertexStride;								// 顶点字节大小
	UINT m_IndexCount;								    // 索引数目	
public:
	Object();
	// 获取位置
	DirectX::XMFLOAT3 GetPosition() const;
	// 设置缓冲区
	template<class VertexType, class IndexType>
	void SetBuffer(ID3D11Device * device, const MeshData<VertexType, IndexType>& meshData)
	{
		{
			// 释放旧资源
			m_pVertexBuffer.Reset();
			m_pIndexBuffer.Reset();

			// 设置顶点缓冲区描述
			m_VertexStride = sizeof(VertexType);
			D3D11_BUFFER_DESC vbd;
			ZeroMemory(&vbd, sizeof(vbd));
			vbd.Usage = D3D11_USAGE_IMMUTABLE;
			vbd.ByteWidth = (UINT)meshData.vertexVec.size() * m_VertexStride;
			vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbd.CPUAccessFlags = 0;
			// 新建顶点缓冲区
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = meshData.vertexVec.data();
			device->CreateBuffer(&vbd, &InitData, m_pVertexBuffer.GetAddressOf());


			// 设置索引缓冲区描述
			m_IndexCount = (UINT)meshData.indexVec.size();
			D3D11_BUFFER_DESC ibd;
			ZeroMemory(&ibd, sizeof(ibd));
			ibd.Usage = D3D11_USAGE_IMMUTABLE;
			ibd.ByteWidth = m_IndexCount * sizeof(IndexType);
			ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			ibd.CPUAccessFlags = 0;
			// 新建索引缓冲区
			InitData.pSysMem = meshData.indexVec.data();
			device->CreateBuffer(&ibd, &InitData, m_pIndexBuffer.GetAddressOf());
		}
	}
	//纹理
	void SetTexture(ID3D11ShaderResourceView * texture);

	void SetWorldMatrix(const DirectX::XMFLOAT4X4& world);

	void XM_CALLCONV SetWorldMatrix(DirectX::FXMMATRIX world);

	// 绘制
	void Draw(ID3D11DeviceContext * deviceContext);

	void SetPostion(DirectX::XMFLOAT3 pos);
};
