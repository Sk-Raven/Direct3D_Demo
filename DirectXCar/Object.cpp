#include <d3d11_1.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include"Cylinder.h"
#include"Light.h"
#include<math.h>
#include"Object.h"
using namespace DirectX;
Object::Object()
	:m_IndexCount(), m_VertexStride()
{
	DirectX::XMStoreFloat4x4(&m_WorldMatrix, DirectX::XMMatrixIdentity());
}

// 获取位置
XMFLOAT3 Object::GetPosition() const
{
	return XMFLOAT3(m_WorldMatrix(3, 0), m_WorldMatrix(3, 1), m_WorldMatrix(3, 2));
}


//纹理
void Object::SetTexture(ID3D11ShaderResourceView * texture)
{
	m_pTexture = texture;
}
// 设置材质
//void SetMaterial(const Material & material);
// 设置矩阵
void Object::SetWorldMatrix(const DirectX::XMFLOAT4X4& world)
{
	m_WorldMatrix = world;
}
void XM_CALLCONV Object::SetWorldMatrix(DirectX::FXMMATRIX world)
{
	XMStoreFloat4x4(&m_WorldMatrix, world);
}
// 绘制
void Object::Draw(ID3D11DeviceContext * deviceContext)
{
	// 设置顶点/索引缓冲区
	UINT strides = m_VertexStride;
	UINT offsets = 0;
	deviceContext->IASetVertexBuffers(0, 1, m_pVertexBuffer.GetAddressOf(), &strides, &offsets);
	deviceContext->IASetIndexBuffer(m_pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);

	// 获取之前已经绑定到渲染管线上的常量缓冲区并进行修改
	Microsoft::WRL::ComPtr<ID3D11Buffer> cBuffer = nullptr;
	deviceContext->VSGetConstantBuffers(0, 1, cBuffer.GetAddressOf());
	CBDrawing cbDrawing;

	// 内部进行转置，这样外部就不需要提前转置了
	DirectX::XMMATRIX W = DirectX::XMLoadFloat4x4(&m_WorldMatrix);
	cbDrawing.world = XMMatrixTranspose(W);
	cbDrawing.worldInvTranspose = XMMatrixInverse(nullptr, W);
	//cbDrawing.material = m_Material;

	// 更新常量缓冲区
	D3D11_MAPPED_SUBRESOURCE mappedData;
	deviceContext->Map(cBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy_s(mappedData.pData, sizeof(CBDrawing), &cbDrawing, sizeof(CBDrawing));
	deviceContext->Unmap(cBuffer.Get(), 0);

	// 设置纹理
	deviceContext->PSSetShaderResources(0, 1, m_pTexture.GetAddressOf());
	// 可以开始绘制
	deviceContext->DrawIndexed(m_IndexCount, 0, 0);
}

void Object::SetPostion(DirectX::XMFLOAT3 pos)
{
	m_WorldMatrix._41 = pos.x;
	m_WorldMatrix._42 = pos.y;
	m_WorldMatrix._43 = pos.z;
}





