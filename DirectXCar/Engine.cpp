#include "Engine.h"
#include <string>
#include"Object.h"
#include<DDSTextureLoader.h>
#include<WICTextureLoader.h>
#include"Vertex.h"
namespace
{
	Engine *p_Engine = nullptr;
}
using namespace DirectX;
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{

	return p_Engine->MsgProc(hwnd, msg, wParam, lParam);
}

Engine::Engine(HINSTANCE hInstance)
{
	hAppHinstance = hInstance;
	m_WindowsWidth = 800;
	m_WindowsHeight = 800;
	hMainWnd = nullptr;
	m_pd3dDevice = nullptr;
	m_pd3dImmediateContext = nullptr;
	m_pSwapChain = nullptr;
	m_pDepthStencilBuffer = nullptr;
	m_pRenderTargetView = nullptr;
	m_pDepthStencilView = nullptr;
	m_IndexCount = 0;
	m_DeltaTime = 0;


	memset(&m_ScreenViewport, 0, sizeof(D3D11_VIEWPORT));
	p_Engine = this;


}

Engine::~Engine()
{
	if (m_pd3dImmediateContext)
		m_pd3dImmediateContext->ClearState();
}

int Engine::Run()
{
	MSG msg = { 0 };

	m_Timer.Init();       //初始化Timer

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_Timer.Tick();
			Update(m_Timer.DeltaTime());     //更新位置
			if (m_DeltaTime < 0.01)         //控制帧率
			{
				m_DeltaTime += m_Timer.DeltaTime();
			}
			else
			{
				m_DeltaTime = 0;
				Draw();
			}

		}
	}

	return (int)msg.wParam;
}

bool Engine::Init()
{
	m_pMouse = std::make_unique<DirectX::Mouse>();
	m_pKeyboard = std::make_unique<DirectX::Keyboard>();
	m_pGamePad = std::make_unique<DirectX::GamePad>();
	try
	{
		WNDCLASS wc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = hAppHinstance;
		wc.lpfnWndProc = MainWndProc;
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.hIcon = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName = 0;
		wc.lpszClassName = L"demo";
		RegisterClass(&wc);
		hMainWnd = CreateWindow(L"demo", L"WASD开车空格刹车12切换视角",
			WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 800, 0, 0, hAppHinstance, 0);
		ShowWindow(hMainWnd, SW_SHOW);
		UpdateWindow(hMainWnd);


		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
		};
		D3D_FEATURE_LEVEL featureLevel;
		UINT createDeviceFlags = 0;
		D3D_DRIVER_TYPE driverTypes = D3D_DRIVER_TYPE_HARDWARE;
		D3D11CreateDevice(nullptr, driverTypes, nullptr, createDeviceFlags, featureLevels, 2,
			D3D11_SDK_VERSION, m_pd3dDevice.GetAddressOf(), &featureLevel, m_pd3dImmediateContext.GetAddressOf());

		ComPtr<IDXGIDevice> dxgiDevice = nullptr;
		ComPtr<IDXGIAdapter> dxgiAdapter = nullptr;
		ComPtr<IDXGIFactory1> dxgiFactory1 = nullptr;	// D3D11.0
		ComPtr<IDXGIFactory2> dxgiFactory2 = nullptr;	// D3D11.1

		//获取创建 D3D设备 的 DXGI工厂
		m_pd3dDevice.As(&dxgiDevice);
		dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
		dxgiAdapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgiFactory1.GetAddressOf()));

		// 切换到D3D11.1
		dxgiFactory1.As(&dxgiFactory2);
		m_pd3dDevice.As(&m_pd3dDevice1);
		m_pd3dImmediateContext.As(&m_pd3dImmediateContext1);
		// 填充各种结构体用以描述交换链
		DXGI_SWAP_CHAIN_DESC1 sd;
		memset(&sd, 0, sizeof(sd));
		sd.Width = m_WindowsWidth;
		sd.Height = m_WindowsHeight;
		sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.BufferCount = 1;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		sd.Flags = 0;

		DXGI_SWAP_CHAIN_FULLSCREEN_DESC fd;
		fd.RefreshRate.Numerator = 60;
		fd.RefreshRate.Denominator = 1;
		fd.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		fd.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		fd.Windowed = TRUE;
		// 为当前窗口创建交换链
		dxgiFactory2->CreateSwapChainForHwnd(m_pd3dDevice.Get(), hMainWnd, &sd, &fd, nullptr, m_pSwapChain1.GetAddressOf());
		m_pSwapChain1.As(&m_pSwapChain);
		OnResize();
	}
	catch (const std::exception&)
	{
		return false;
	}
	LoadShader();
	LoadObject();
	m_pMouse->SetWindow(hMainWnd);
	m_pMouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
	return true;
}

void Engine::Update(float dt)
{

	Mouse::State mouseState = m_pMouse->GetState();
	Mouse::State lastMouseState = m_MouseTracker.GetLastState();

	Keyboard::State keyState = m_pKeyboard->GetState();
	Keyboard::State lastKeyState = m_KeyboardTracker.GetLastState();

	GamePad::State gamepadState = m_pGamePad->GetState(0);
	GamePad::State lastGamepadState = m_GamePadTracker.GetLastState();



	auto cam1st = std::dynamic_pointer_cast<FirstPersonCamera>(m_pCamera);
	auto cam3rd = std::dynamic_pointer_cast<ThirdPersonCamera>(m_pCamera);

	m_Car.Run();
	m_Wheel[0].Binding(m_Car.GetPosition(), m_Car.Getm_theta(), m_Car.getSpeed(), 0);
	m_Wheel[1].Binding(m_Car.GetPosition(), m_Car.Getm_theta(), m_Car.getSpeed(), 1);
	m_Wheel[2].Binding(m_Car.GetPosition(), m_Car.Getm_theta(), m_Car.getSpeed(), 2);
	m_Wheel[3].Binding(m_Car.GetPosition(), m_Car.Getm_theta(), m_Car.getSpeed(), 3);

	// 摄像机模式切换
	if (keyState.IsKeyDown(Keyboard::D1) && m_CameraMode == CameraMode::ThirdPerson)
	{
		if (!cam1st)
		{
			cam1st.reset(new FirstPersonCamera);
			cam1st->SetFrustum(XM_PI / 3, (float)m_WindowsWidth / m_WindowsHeight, 0.5f, 1000.0f);
			m_pCamera = cam1st;
		}

		cam1st->LookAt(m_Car.GetPosition(),
			XMFLOAT3(-cos(m_Car.Getm_theta()), 0.0f, sin(m_Car.Getm_theta())),
			XMFLOAT3(0.0f, 1.0f, 0.0f));
		cam1st->SetPosition(m_Car.GetPosition());
		m_CameraMode = CameraMode::FirstPerson;
	}
	else if (keyState.IsKeyDown(Keyboard::D2) && m_CameraMode == CameraMode::FirstPerson)
	{
		if (!cam3rd)
		{
			cam3rd.reset(new ThirdPersonCamera);
			cam3rd->SetFrustum(XM_PI / 3, (float)m_WindowsWidth / m_WindowsHeight, 0.5f, 1000.0f);
			m_pCamera = cam3rd;
		}
		XMFLOAT3 target = m_Car.GetPosition();
		cam3rd->SetTarget(target);
		cam3rd->SetDistance(8.0f);
		cam3rd->SetDistanceMinMax(3.0f, 20.0f);

		m_CameraMode = CameraMode::ThirdPerson;
	}



	if (m_CameraMode == CameraMode::FirstPerson)
	{
		// 第一人称/自由摄像机的操作


		if (keyState.IsKeyDown(Keyboard::W))
		{
			m_Car.Addspeed();
		}
		if (keyState.IsKeyDown(Keyboard::S))
		{
			m_Car.Reducespeed();
		}
		if (keyState.IsKeyDown(Keyboard::A))
		{
			m_Car.RotationY(dt * 0.75f);
			m_Wheel[2].SetSteerAngle(-XM_PIDIV4);
			m_Wheel[3].SetSteerAngle(-XM_PIDIV4);

		}
		if (keyState.IsKeyDown(Keyboard::D))
		{
			m_Car.RotationY(-dt * 0.75f);
			m_Wheel[2].SetSteerAngle(XM_PIDIV4);
			m_Wheel[3].SetSteerAngle(XM_PIDIV4);

		}
		if (!keyState.IsKeyDown(Keyboard::A) && !keyState.IsKeyDown(Keyboard::D))
		{
			m_Wheel[2].SetSteerAngle(0);
			m_Wheel[3].SetSteerAngle(0);
		}



		XMFLOAT3 adjustedPos;
		DirectX::XMStoreFloat3(&adjustedPos, XMVectorClamp(cam1st->GetPositionXM(), XMVectorSet(-1000.0f, 1000.0f, -1000.0f, 1000.0f), XMVectorReplicate(1000.0f)));
		cam1st->SetPosition(adjustedPos);


		//视野旋转，防止开始的差值过大导致的突然旋转
		cam1st->LookTo(m_Car.GetPosition(),
			XMFLOAT3(-cos(m_Car.Getm_theta()), 0.0f, sin(m_Car.Getm_theta())),
			XMFLOAT3(0.0f, 1.0f, 0.0f));
		cam1st->Pitch(mouseState.y * dt * 1.25f);
		cam1st->RotateY(mouseState.x * dt * 1.25f);
	}
	else if (m_CameraMode == CameraMode::ThirdPerson)
	{
		// 第三人称摄像机的操作

		cam3rd->SetTarget(m_Car.GetPosition());
		// 绕物体旋转
		cam3rd->RotateX(mouseState.y * dt * 2.25f);
		cam3rd->RotateY(mouseState.x * dt * 2.25f);
		cam3rd->Approach(-mouseState.scrollWheelValue / 120 * 1.0f);
		if (keyState.IsKeyDown(Keyboard::W))
		{
			m_Car.Addspeed();
		}
		if (keyState.IsKeyDown(Keyboard::S))
		{
			m_Car.Reducespeed();
		}
		if (keyState.IsKeyDown(Keyboard::Space))
		{
			m_Car.Brake();
		}
		if (keyState.IsKeyDown(Keyboard::A))
		{
			m_Car.RotationY(dt * 0.75f);
			m_Wheel[2].SetSteerAngle(-XM_PIDIV4);
			m_Wheel[3].SetSteerAngle(-XM_PIDIV4);

		}
		if (keyState.IsKeyDown(Keyboard::D))
		{
			m_Car.RotationY(-dt * 0.75f);
			m_Wheel[2].SetSteerAngle(XM_PIDIV4);
			m_Wheel[3].SetSteerAngle(XM_PIDIV4);

		}
		if (!keyState.IsKeyDown(Keyboard::A) && !keyState.IsKeyDown(Keyboard::D))
		{
			m_Wheel[2].SetSteerAngle(0);
			m_Wheel[3].SetSteerAngle(0);
		}

	}

	// 退出程序
	if (keyState.IsKeyDown(Keyboard::Escape))
		SendMessage(GetMainWnd(), WM_DESTROY, 0, 0);

	//天空盒移动
	m_SkyBox.SetPostion(m_pCamera->GetPosition());
	// 更新观察矩阵
	m_pCamera->UpdateViewMatrix();
	DirectX::XMStoreFloat4(&m_Frame.eyePos, m_pCamera->GetPositionXM());
	m_Frame.view = XMMatrixTranspose(m_pCamera->GetViewXM());


	// 重置滚轮值
	m_pMouse->ResetScrollWheelValue();




	D3D11_MAPPED_SUBRESOURCE mappedData;
	m_pd3dImmediateContext->Map(m_pConstantBuffers[1].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy_s(mappedData.pData, sizeof(Frame), &m_Frame, sizeof(Frame));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[1].Get(), 0);


}

void Engine::Draw()
{
	m_pd3dImmediateContext->ClearRenderTargetView(m_pRenderTargetView.Get(), reinterpret_cast<const float*>(&Colors::White));
	m_pd3dImmediateContext->ClearDepthStencilView(m_pDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	m_Car.Draw(m_pd3dImmediateContext.Get());
	m_Floor.Draw(m_pd3dImmediateContext.Get());
	m_Wheel[0].Draw(m_pd3dImmediateContext.Get());
	m_Wheel[1].Draw(m_pd3dImmediateContext.Get());
	m_Wheel[2].Draw(m_pd3dImmediateContext.Get());
	m_Wheel[3].Draw(m_pd3dImmediateContext.Get());
	m_Object[0].Draw(m_pd3dImmediateContext.Get());
	m_Object[1].Draw(m_pd3dImmediateContext.Get());
	m_SkyBox.Draw(m_pd3dImmediateContext.Get());
	m_pSwapChain->Present(0, 0);
}



void Engine::LoadObject()
{

	D3D11_BUFFER_DESC cbd;
	memset(&cbd, 0, sizeof(cbd));
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	// 新建用于VS和PS的常量缓冲区
	cbd.ByteWidth = sizeof(CBDrawing);
	m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[0].GetAddressOf());
	cbd.ByteWidth = sizeof(Frame);
	m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[1].GetAddressOf());
	cbd.ByteWidth = sizeof(CBChangesOnResize);
	m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[2].GetAddressOf());
	cbd.ByteWidth = sizeof(Light);
	m_pd3dDevice->CreateBuffer(&cbd, nullptr, m_pConstantBuffers[3].GetAddressOf());



	// 初始化车
	ComPtr<ID3D11ShaderResourceView> texture;
	CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\WoodCrate.dds", nullptr, texture.GetAddressOf());
	m_Car.SetBuffer(m_pd3dDevice.Get(), CreateCylinder());
	m_Car.SetTexture(texture.Get());

	//初始化地板
	CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\floor.dds", nullptr, texture.ReleaseAndGetAddressOf());
	m_Floor.SetBuffer(m_pd3dDevice.Get(),
		CreatePlane(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(20.0f, 20.0f), XMFLOAT2(5.0f, 5.0f)));
	m_Floor.SetTexture(texture.Get());
	m_Floor.SetWorldMatrix(XMMatrixTranslation(0.0f, -1.0f, 0.0f));

	//初始化天空盒
	CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\flare.dds", nullptr, texture.ReleaseAndGetAddressOf());
	m_SkyBox.SetBuffer(m_pd3dDevice.Get(),
		CreateBox(200.0f, 200.0f, 200.0f));
	m_SkyBox.SetTexture(texture.Get());
	m_SkyBox.SetWorldMatrix(XMMatrixTranslation(0.0f, -1.0f, 0.0f));

	//初始化车轮
	m_Wheel.resize(4);
	CreateDDSTextureFromFile(m_pd3dDevice.Get(), L"Texture\\WoodCrate.dds", nullptr, texture.ReleaseAndGetAddressOf());

	m_Wheel[0].SetBuffer(m_pd3dDevice.Get(), CreateCylinder(0.4f, 0.2f));
	m_Wheel[0].SetTexture(texture.Get());
	XMMATRIX world0 = XMMatrixRotationY(XM_PIDIV2) * XMMatrixTranslation(1.0f, -0.5f, 2.0f);
	m_Wheel[0].SetWorldMatrix(world0);

	m_Wheel[1].SetBuffer(m_pd3dDevice.Get(), CreateCylinder(0.4f, 0.2f));
	m_Wheel[1].SetTexture(texture.Get());
	XMMATRIX world1 = XMMatrixRotationY(XM_PIDIV2)* XMMatrixTranslation(-1.0f, -0.5f, 2.0f);
	m_Wheel[1].SetWorldMatrix(world1);

	m_Wheel[2].SetBuffer(m_pd3dDevice.Get(), CreateCylinder(0.4f, 0.2f));
	m_Wheel[2].SetTexture(texture.Get());
	XMMATRIX world2 = XMMatrixRotationY(XM_PIDIV2)* XMMatrixTranslation(-1.0f, -0.5f, -2.0f);
	m_Wheel[2].SetWorldMatrix(world2);

	m_Wheel[3].SetBuffer(m_pd3dDevice.Get(), CreateCylinder(0.4f, 0.2f));
	m_Wheel[3].SetTexture(texture.Get());
	XMMATRIX world3 = XMMatrixRotationY(XM_PIDIV2)* XMMatrixTranslation(1.0f, -0.5f, -2.0f);
	m_Wheel[3].SetWorldMatrix(world3);

	//初始化点缀
	m_Object.resize(2);
	m_Object[0].SetBuffer(m_pd3dDevice.Get(), CreateCylinder());
	m_Object[0].SetTexture(texture.Get());
	XMMATRIX world4 = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(5.0f, 1.0f, 5.0f);
	m_Object[0].SetWorldMatrix(world4);

	m_Object[1].SetBuffer(m_pd3dDevice.Get(), CreateCylinder());
	m_Object[1].SetTexture(texture.Get());
	XMMATRIX world5 = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixTranslation(-5.0f, 1.0f, -5.0f);
	m_Object[1].SetWorldMatrix(world5);


	for (int i = 0; i < 4; ++i)
	{
		m_Wheel[i].SetTexture(texture.Get());
	}

	// 初始化采样器状态
	D3D11_SAMPLER_DESC sampDesc;
	memset(&sampDesc, 0, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	sampDesc.MaxAnisotropy = 16;
	m_pd3dDevice->CreateSamplerState(&sampDesc, m_pSamplerState.GetAddressOf());


	m_CameraMode = CameraMode::FirstPerson;
	auto camera = std::shared_ptr<FirstPersonCamera>(new FirstPersonCamera);
	m_pCamera = camera;
	camera->SetViewPort(0.0f, 0.0f, (float)m_WindowsWidth, (float)m_WindowsHeight);
	camera->LookAt(XMFLOAT3(), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));

	m_pCamera->SetFrustum(XM_PI / 3, (float)m_WindowsWidth / m_WindowsHeight, 0.5f, 1000.0f);
	m_CBOnResize.proj = XMMatrixTranspose(m_pCamera->GetProjXM());


	// 环境光
	m_Light.dirLight[0].ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_Light.dirLight[0].diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_Light.dirLight[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_Light.dirLight[0].direction = XMFLOAT3(0.0f, -1.0f, 0.0f);
	// 点光
	m_Light.pointLight[0].position = XMFLOAT3(5.0f, -1.0f, 0.0f);
	m_Light.pointLight[0].ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_Light.pointLight[0].diffuse = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_Light.pointLight[0].specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_Light.pointLight[0].att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	m_Light.pointLight[0].range = 25.0f;
	// 聚光灯
	m_Light.spotLight[0].position = XMFLOAT3(0.0f, -1.0f, 5.0f);
	m_Light.spotLight[0].direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_Light.spotLight[0].ambient = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	m_Light.spotLight[0].diffuse = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.spotLight[0].specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light.spotLight[0].att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	m_Light.spotLight[0].spot = 12.0f;
	m_Light.spotLight[0].range = 10000.0f;

	m_Light.numDirLight = 1;
	m_Light.numPointLight = 1;
	m_Light.numSpotLight = 1;
	// 初始化材质
	m_Light.material.ambient = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_Light.material.diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	m_Light.material.specular = XMFLOAT4(0.1f, 0.1f, 0.1f, 50.0f);


	D3D11_MAPPED_SUBRESOURCE mappedData;
	m_pd3dImmediateContext->Map(m_pConstantBuffers[2].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy_s(mappedData.pData, sizeof(CBChangesOnResize), &m_CBOnResize, sizeof(CBChangesOnResize));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[2].Get(), 0);

	m_pd3dImmediateContext->Map(m_pConstantBuffers[3].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy_s(mappedData.pData, sizeof(Light), &m_Light, sizeof(Light));
	m_pd3dImmediateContext->Unmap(m_pConstantBuffers[3].Get(), 0);


	// 给渲染管线各个阶段绑定好所需资源
	// 设置图元类型，设定输入布局
	m_pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_pd3dImmediateContext->IASetInputLayout(m_pVertexLayout.Get());
	// 默认绑定3D着色器
	m_pd3dImmediateContext->VSSetShader(m_pVertexShader.Get(), nullptr, 0);
	// 预先绑定各自所需的缓冲区，其中每帧更新的缓冲区需要绑定到两个缓冲区上
	m_pd3dImmediateContext->VSSetConstantBuffers(0, 1, m_pConstantBuffers[0].GetAddressOf());
	m_pd3dImmediateContext->VSSetConstantBuffers(1, 1, m_pConstantBuffers[1].GetAddressOf());
	m_pd3dImmediateContext->VSSetConstantBuffers(2, 1, m_pConstantBuffers[2].GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(1, 1, m_pConstantBuffers[1].GetAddressOf());
	m_pd3dImmediateContext->PSSetConstantBuffers(3, 1, m_pConstantBuffers[3].GetAddressOf());
	m_pd3dImmediateContext->PSSetShader(m_pPixelShader.Get(), nullptr, 0);
	m_pd3dImmediateContext->PSSetSamplers(0, 1, m_pSamplerState.GetAddressOf());



}

void Engine::LoadShader() {
	ComPtr<ID3DBlob> blob;
	// 创建顶点着色器(3D)
	CreateShaderFromFile(L"HLSL\\Basic_VS_3D.cso", L"HLSL\\Basic_VS_3D.hlsl", "VS_3D", "vs_5_0", blob.ReleaseAndGetAddressOf());
	m_pd3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pVertexShader.GetAddressOf());
	// 创建顶点布局(3D)
	m_pd3dDevice->CreateInputLayout(VertexPosNormalTex::inputLayout, ARRAYSIZE(VertexPosNormalTex::inputLayout),
		blob->GetBufferPointer(), blob->GetBufferSize(), m_pVertexLayout.GetAddressOf());
	// 创建像素着色器(3D)
	CreateShaderFromFile(L"HLSL\\Basic_PS_3D.cso", L"HLSL\\Basic_PS_3D.hlsl", "PS_3D", "ps_5_0", blob.ReleaseAndGetAddressOf());
	m_pd3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_pPixelShader.GetAddressOf());
}


void Engine::OnResize()
{



	// 释放渲染管线输出用到的相关资源
	m_pRenderTargetView.Reset();
	m_pDepthStencilView.Reset();
	m_pDepthStencilBuffer.Reset();

	// 重设交换链并且重新创建渲染目标视图
	ComPtr<ID3D11Texture2D> backBuffer;
	m_pSwapChain->ResizeBuffers(1, m_WindowsWidth, m_WindowsHeight, DXGI_FORMAT_B8G8R8A8_UNORM, 0);	// 注意此处DXGI_FORMAT_B8G8R8A8_UNORM
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	m_pd3dDevice->CreateRenderTargetView(backBuffer.Get(), nullptr, m_pRenderTargetView.GetAddressOf());


	backBuffer.Reset();


	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = m_WindowsWidth;
	depthStencilDesc.Height = m_WindowsHeight;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// 要使用 4X MSAA?
	if (m_Enable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m_4xMsaaQuality - 1;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}


	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	// 创建深度缓冲区以及深度模板视图
	m_pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, m_pDepthStencilBuffer.GetAddressOf());
	m_pd3dDevice->CreateDepthStencilView(m_pDepthStencilBuffer.Get(), nullptr, m_pDepthStencilView.GetAddressOf());


	// 将渲染目标视图和深度/模板缓冲区结合到管线
	m_pd3dImmediateContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), m_pDepthStencilView.Get());

	// 设置视口变换
	m_ScreenViewport.TopLeftX = 0;
	m_ScreenViewport.TopLeftY = 0;
	m_ScreenViewport.Width = static_cast<float>(m_WindowsWidth);
	m_ScreenViewport.Height = static_cast<float>(m_WindowsHeight);
	m_ScreenViewport.MinDepth = 0.0f;
	m_ScreenViewport.MaxDepth = 1.0f;

	m_pd3dImmediateContext->RSSetViewports(1, &m_ScreenViewport);




}



HINSTANCE Engine::GetAppHinstance()
{
	return hAppHinstance;
}

HWND Engine::GetMainWnd()
{
	return hMainWnd;
}

LRESULT Engine::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		// WM_ACTIVATE is sent when the window is activated or deactivated.  
		// We pause the game when the window is deactivated and unpause it 
		// when it becomes active.  
	case WM_ACTIVATE:
		if (LOWORD(wParam) == WA_INACTIVE)
		{
			m_Timer.Stop();
		}
		else
		{
			m_Timer.Start();
		}
		return 0;


		// WM_DESTROY is sent when the window is being destroyed.
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

		// The WM_MENUCHAR message is sent when a menu is active and the user presses 
		// a key that does not correspond to any mnemonic or accelerator key. 
	case WM_MENUCHAR:
		// Don't beep when we alt-enter.
		return MAKELRESULT(0, MNC_CLOSE);

		// Catch this message so to prevent the window from becoming too small.
	case WM_GETMINMAXINFO:
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;

	case WM_INPUT:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_XBUTTONDOWN:

	case WM_LBUTTONUP:
	case WM_MBUTTONUP:
	case WM_RBUTTONUP:
	case WM_XBUTTONUP:

	case WM_MOUSEWHEEL:
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
		m_pMouse->ProcessMessage(msg, wParam, lParam);
		return 0;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYUP:
		m_pKeyboard->ProcessMessage(msg, wParam, lParam);
		return 0;

	case WM_ACTIVATEAPP:
		m_pMouse->ProcessMessage(msg, wParam, lParam);
		m_pKeyboard->ProcessMessage(msg, wParam, lParam);
		return 0;

	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

HRESULT CreateShaderFromFile(
	const WCHAR* csoFileNameInOut,
	const WCHAR* hlslFileName,
	LPCSTR entryPoint,
	LPCSTR shaderModel,
	ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;

	// 寻找是否有已经编译好的顶点着色器
	if (csoFileNameInOut && D3DReadFileToBlob(csoFileNameInOut, ppBlobOut) == S_OK)
	{
		return hr;
	}
	else
	{
		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;

		ID3DBlob* errorBlob = nullptr;
		hr = D3DCompileFromFile(hlslFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, shaderModel,
			dwShaderFlags, 0, ppBlobOut, &errorBlob);
		if (FAILED(hr))
		{
			if (errorBlob != nullptr)
			{
				OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
			}

			errorBlob->Release();
			errorBlob = nullptr;
			return hr;
		}

		// 若指定了输出文件名，则将着色器二进制信息输出
		if (csoFileNameInOut)
		{
			return D3DWriteBlobToFile(*ppBlobOut, csoFileNameInOut, FALSE);
		}
	}

	return hr;
}





