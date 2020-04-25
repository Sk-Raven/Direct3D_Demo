#pragma once
#include <d3d11_1.h>
#include <DirectXColors.h>
#include"Light.h"

struct ConstantBuffer
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

struct VSConstantBuffer
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
	DirectX::XMMATRIX worldInvTranspose;

};

struct PSConstantBuffer
{
	DirectionalLight dirLight[10];
	PointLight pointLight[10];
	SpotLight spotLight[10];
	Material material;
	int numDirLight;
	int numPointLight;
	int numSpotLight;
	float pad;
	DirectX::XMFLOAT4 eyePos;
};

struct CBDrawing
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX worldInvTranspose;
};

struct Frame
{
	DirectX::XMMATRIX view;
	DirectX::XMFLOAT4 eyePos;
};

struct CBChangesOnResize
{
	DirectX::XMMATRIX proj;
};

HRESULT CreateShaderFromFile(
	const WCHAR* csoFileNameInOut,
	const WCHAR* hlslFileName,
	LPCSTR entryPoint,
	LPCSTR shaderModel,
	ID3DBlob** ppBlobOut);                              //从文件里加载Shader

