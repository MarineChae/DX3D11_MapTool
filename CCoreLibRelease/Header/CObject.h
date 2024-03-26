#pragma once
#include"CDxObject.h"


class CObject : public CDxObject
{
public:

	CRect m_RT;
	CBox m_CollisionBox;
public:

	void SetRect(TVector2 p, float width, float height);


public:
	TMatrix m_WolrdMatrix;
	TMatrix m_ViewMatrix;
	TMatrix m_ProjMatrix;

public:
	float m_TexAccumulatedTime=0.0f;
	
public:
	TVector3 m_vPos;
	TVector3 m_vScale;
	TVector3 m_vRotate;

	virtual bool SetPos(TVector3 Pos);
	virtual void SetScale(TVector3 Pos);
	virtual void Move(double fSecond) {};

public:
	virtual bool CreateVertexData() { return true; };
	virtual bool CreateIndexData() { return true; };

	virtual void UpdataMatrix();
	//virtual bool Create(std::wstring FileName, std::wstring ShaderFileName,float Delay);
	virtual bool Create(std::wstring FileName, std::wstring ShaderFileName);
	virtual void SetMatrix(TMatrix* WolrdMatrix , TMatrix* ViewMatrix, TMatrix* ProjMatrix);
	
	
public:


	virtual bool Init();
	virtual bool Frame();
	virtual bool Render();
	virtual bool Release();

public:
	CObject()
	{
		m_vPos = TVector3(0, 0, 0);
		m_vScale = TVector3(1, 1, 1);
		m_vRotate = TVector3(0, 0, 0);
	}

};

