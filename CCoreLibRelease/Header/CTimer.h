#pragma once
#pragma comment(lib,"winmm.lib")
#include<windows.h>
#include<iostream>
#include<chrono>

class CTimer
{
public:
	double m_fGameTime;
	double m_fSecondPerFrame;
	double m_fFramePerSecond;
	std::chrono::high_resolution_clock::time_point m_dwBeforeTime;
	int m_iFPS=0;


public:
	int GetFPS();

public:
	bool  Init();
	bool  Frame();
	bool  Render();
	bool Release();



};

