#pragma once
#include"Object.h"
class Wheel : public Object
{
private:
	float SteerAngle;                                   //转向角
	float RollAngle;                                    //滚动角
public:
	Wheel();

	void Binding(DirectX::XMFLOAT3 pos, float p_theta, int speed, int num); //绑定到车身上

	void SetSteerAngle(float in_Angle); //设置转向角
};