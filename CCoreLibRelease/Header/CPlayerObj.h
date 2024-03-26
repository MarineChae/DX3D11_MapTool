#pragma once
#include"CSpriteObj.h"
#include"CInput.h"
class CPlayerObj : public  CSpriteUVObj
{
public:
	long long HealthPoint;
public:
	bool m_bVaild = true;
	TVector3  m_vDirection = { 0,0,0 };
	bool IsMove = false;
	bool Roll = false;
	bool Jump = false;
	bool Drop = false;
	bool FaceLeft = true;
	bool FaceRight = false;
	TVector3 BeforePos = { 0,0,0 };
	double MaxSpeed = 500.0f;
	double MoveSpeed = 0.0f;
	double MaxDropSpeed = 1100.0f;
	double DropSpeed = 400.0f;
	double MinSpeed = -500.0f;

public:

	
	virtual void Move(double fSecond);
	virtual void Turn();
	virtual bool Render();

public:


};