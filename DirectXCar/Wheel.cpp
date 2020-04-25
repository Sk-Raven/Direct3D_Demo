#include"Wheel.h"
using namespace DirectX;

Wheel::Wheel()
{
	Object();
	SteerAngle = 0.0f;
	RollAngle = 0.0f;
}

void Wheel::Binding(DirectX::XMFLOAT3 pos, float p_theta, int speed, int num)
{
	XMMATRIX world;
	if (speed != 0)
	{
		RollAngle = XMScalarModAngle(RollAngle - 0.00002f*speed); //车轮滚动
	}

	world = XMMatrixRotationX(RollAngle) * XMMatrixRotationY(SteerAngle); //车轮转向

	if (num == 0)                                                        //不同车轮绑定不同位置
	{
		world *= XMMatrixTranslation(1.0f, -0.5f, 2.0f);
	}
	else if (num == 1)
	{
		world *= XMMatrixTranslation(-1.0f, -0.5f, 2.0f);
	}
	else if (num == 2)
	{
		world *= XMMatrixTranslation(-1.0f, -0.5f, -2.0f);
	}
	else if (num == 3)
	{
		world *= XMMatrixTranslation(1.0f, -0.5f, -2.0f);
	}

	SetWorldMatrix(world*XMMatrixRotationY(p_theta + XM_PIDIV2));      //平移过去
	m_WorldMatrix._41 += pos.x;
	m_WorldMatrix._42 += pos.y;
	m_WorldMatrix._43 += pos.z;
}


void Wheel::SetSteerAngle(float in_Angle)
{
	SteerAngle = in_Angle;
}