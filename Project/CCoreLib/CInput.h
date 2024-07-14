#pragma once
#include "CDefine.h"



enum KEY_STATE
{
	KEY_NONE = 0,

	KEY_UP = 1,

	KEY_PUSH =2,

	KEY_HOLD = 3,


};


class CInput
{

public:
	TVector3 m_vOffSet = { 0,0,0 };
	POINT m_MousePos;
	POINT m_BeforeMousePos;
	DWORD m_dwKeyState[256] = { 0, };

public:
	static CInput& GetInstance()
	{
		static CInput input;

		return input;
	}

public:
	bool		Init();
	bool		Frame();
	bool		Render();
	bool		Release();

private:
	CInput() {};

public:
	virtual~CInput() {};

};

