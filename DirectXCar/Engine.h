#pragma once
#include <d3d11_1.h>
#include"Timer.h"
#include"Helper.h"
#include"Mouse.h"
#include"Keyboard.h"
#include"GamePad.h"
#include"Cylinder.h"
#include"Light.h"
#include"Vertex.h"
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include"Object.h"
#include"Camera.h"
#include"Car.h"
#include"Wheel.h"
#include <string>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")
#pragma comment(lib, "winmm.lib")
enum class CameraMode { FirstPerson, ThirdPerson };   //相机人称
class Engine
{
public:

	Engine(HINSTANCE hInstance);
	virtual ~Engine();

	HINSTANCE GetAppHinstance();              // 获取应用实例的句柄
	HWND      GetMainWnd();                 // 获取主窗口句柄
	LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	int Run();                                // 运行程序，进行游戏循环
	bool Init();                      // 初始化设备
	void LoadShader();               //加载着色器
	void LoadObject();             //加载物体   
	void Update(float dt);   // 更新位置
	void Draw();            // 绘制画面
	void OnResize();       //窗口绘制


private:

	HINSTANCE hAppHinstance;        // 应用实例句柄
	HWND      hMainWnd;        // 主窗口句柄
	bool	  m_Enable4xMsaa;	 // 是否开启4倍多重采样
	UINT      m_4xMsaaQuality;   // MSAA支持的质量等级
	Timer m_Timer;               //计时器


	template <class T>
	using ComPtr = Microsoft::WRL::ComPtr<T>;
	// Direct3D 11
	ComPtr<ID3D11Device> m_pd3dDevice;							    // D3D11设备
	ComPtr<ID3D11DeviceContext> m_pd3dImmediateContext;			    // D3D11设备上下文
	ComPtr<IDXGISwapChain> m_pSwapChain;							// D3D11交换链
	// Direct3D 11.1
	ComPtr<ID3D11Device1> m_pd3dDevice1;							// D3D11.1设备
	ComPtr<ID3D11DeviceContext1> m_pd3dImmediateContext1;			// D3D11.1设备上下文
	ComPtr<IDXGISwapChain1> m_pSwapChain1;						    // D3D11.1交换链
	ComPtr<ID3D11Texture2D> m_pDepthStencilBuffer;        // 深度模板缓冲区
	ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;   // 渲染目标视图
	ComPtr<ID3D11DepthStencilView> m_pDepthStencilView;   // 深度模板视图
	D3D11_VIEWPORT m_ScreenViewport;
	ComPtr<ID3D11InputLayout> m_pVertexLayout;	// 顶点输入布局
	ComPtr<ID3D11Buffer> m_pVertexBuffer;		// 顶点缓冲区
	ComPtr<ID3D11Buffer> m_pIndexBuffer;			// 索引缓冲区
	ComPtr<ID3D11Buffer> m_pConstantBuffers[4];		    // 常量缓冲区



	ComPtr<ID3D11VertexShader> m_pVertexShader;				    // 顶点着色器
	ComPtr<ID3D11PixelShader> m_pPixelShader;				    // 像素着色器


	Frame m_Frame;							                // 该缓冲区存放仅在每一帧进行更新的变量
	CBChangesOnResize m_CBOnResize;							    // 该缓冲区存放仅在窗口大小变化时更新的变量
	Light m_Light;								    // 灯光
	ComPtr<ID3D11SamplerState> m_pSamplerState;				    // 采样器状态
	UINT m_IndexCount;                              // 绘制物体的索引数组大小



	VSConstantBuffer m_VSConstantBuffer;            // 用于修改用于VS的GPU常量缓冲区的变量
	PSConstantBuffer m_PSConstantBuffer;			// 用于修改用于PS的GPU常量缓冲区的变量
	DirectionalLight m_DirLight;					// 默认环境光

	int m_WindowsWidth;                               //窗口宽    
	int m_WindowsHeight;                              //窗口高

	std::unique_ptr<DirectX::Mouse> m_pMouse;                      //鼠标
	DirectX::Mouse::ButtonStateTracker m_MouseTracker;

	std::unique_ptr<DirectX::Keyboard> m_pKeyboard;					// 键盘
	DirectX::Keyboard::KeyboardStateTracker m_KeyboardTracker;

	std::unique_ptr<DirectX::GamePad> m_pGamePad;                   //手柄
	DirectX::GamePad::ButtonStateTracker m_GamePadTracker;


	Car m_Car;									    // 车身
	Object m_Floor;										    // 地板
	std::vector<Wheel> m_Wheel;							// 车轮
	Object m_SkyBox;                                        // 天空盒    
	std::vector<Object> m_Object;                         //点缀物

	std::shared_ptr<Camera> m_pCamera;
	CameraMode m_CameraMode;

	float m_DeltaTime;                                  //距离上一个帧生成的时间

};



