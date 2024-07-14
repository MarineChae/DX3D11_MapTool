#include "CTimer.h"
#include "CWriter.h"
double g_fSecondPerFrame = 0.0f;
double g_fGameTime = 0.0f;

//타이머 초기화함수
bool  CTimer::Init()
{

	m_fGameTime = 0.0f;
	m_fSecondPerFrame = 0.0f;
	m_fFramePerSecond = 0.0f;
	m_dwBeforeTime = std::chrono::high_resolution_clock::now();
	return true;
}
//프레임 수를 1초마다 계산하여 넘겨줌
int CTimer::GetFPS()
{
	static int FPS = 0;
	if (m_fFramePerSecond >= 1.0f)
	{
		m_fFramePerSecond -= 1.0f;
		m_iFPS=FPS ;
		FPS = 0;

	}
	FPS++;

	return m_iFPS;
}
// 매프레임 시간을 계산
bool  CTimer::Frame()
{
	auto dwCurrentTime = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> m_fSecondPerFrame = dwCurrentTime - m_dwBeforeTime;
	g_fSecondPerFrame = m_fSecondPerFrame.count();
	g_fGameTime = m_fGameTime += g_fSecondPerFrame;
	m_fFramePerSecond += g_fSecondPerFrame;
	m_dwBeforeTime = dwCurrentTime;


	return true;
}
bool  CTimer::Render()
{

	std::wstring msg = L"[FPS]";
	msg += std::to_wstring(GetFPS());
	msg += L"[GT]";
	msg += std::to_wstring(m_fGameTime);

	CWriter::GetInstance().AddText(msg,0,20,D2D1::ColorF(1,1,0,1));

	

	
	return true;
}
bool  CTimer::Release()
{
	return true;
}