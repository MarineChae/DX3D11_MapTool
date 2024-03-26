#pragma once
#include"TMath.h"


class ImGuiManager
{
public:

	bool show_demo_window = true;
	bool show_another_window = true;
	TVector4 clear_color = { 0.0f ,0.0f, 0.0f, 0.0f };
	

public:
	static ImGuiManager& GetInstance()
	{
		static ImGuiManager Shader;
		return Shader;

	}
public:
	bool Init();
	bool Frame();
	bool Render();
	bool Release();

	bool test();

private:
	ImGuiManager();

public:
	virtual ~ImGuiManager();

};

