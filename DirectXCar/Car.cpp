#include"Car.h"
using namespace DirectX;

Car::Car()
{
	Object();
	theta = 0.0f;
	speed = 0;
}
void Car::Run()
{
	XMFLOAT3 temp = GetPosition();
	float dx = cos(theta); //计算分量
	float dz = sin(theta);
	//先在原点旋转再平移
	Object::SetWorldMatrix(XMMatrixRotationY(theta)* XMMatrixTranslation(temp.x - 0.00001f*dx*speed, temp.y, temp.z + 0.00001f*dz*speed));
}

void Car::Addspeed()
{
	if (speed < 2)
		speed++;
}

void Car::Reducespeed()
{
	if (speed > -1)
		speed--;
}

void Car::Brake()
{
	speed = 0;
}

int Car::getSpeed()
{
	return speed;
}

float Car::Getm_theta()
{
	return theta;
}

void Car::RotationY(float dt)
{
	theta = DirectX::XMScalarModAngle(theta - dt);
	DirectX::XMFLOAT3 temp = GetPosition();
	float r = (float)sqrt(5);
	SetWorldMatrix(DirectX::XMMatrixRotationY(theta)* DirectX::XMMatrixTranslation(temp.x, temp.y, temp.z)); //先在原点旋转，再平移过去

}
