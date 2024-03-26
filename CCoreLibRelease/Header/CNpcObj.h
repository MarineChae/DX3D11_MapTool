#pragma once
#include "CPlaneObj.h"

class CNpcObj : public CPlaneObj
{
public:
	TVector3  m_vDirection = { 1,1,0 };


public:
	virtual void Move(double fSecond);
	virtual bool PreRender();
	virtual bool Render();
	virtual bool PostRender();
	virtual bool Release();
	CNpcObj()
	{
		m_vDirection = { randstep(-1, +1), randstep(-1, +1), 0 };
	}
};

