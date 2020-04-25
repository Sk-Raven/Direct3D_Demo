#pragma once
#include"Object.h"
class Car : public Object
{
private:
	int speed;		//速度
	float theta;    //车身旋转角度
public:
	Car();

	void Run();   //移动

	void Addspeed();	//加速

	void Reducespeed();	//减速

	void Brake();        //刹车

	int getSpeed();	   //获取时速

	float Getm_theta(); //获取车身旋转角度

	void RotationY(float dt); //进行车身旋转

};