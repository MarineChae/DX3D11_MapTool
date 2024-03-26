#pragma once
#include"CCore.h"

class Sample : public CCore
{

public:
	
	bool Init();
	bool Frame();
	bool Render();
	bool Release();
public:
	bool CreateResource();
	bool DeleteResource();

};
